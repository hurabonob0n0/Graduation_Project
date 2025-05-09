#pragma once
#include "GameInstance.h"
#include "Timer.h"


using Microsoft::WRL::ComPtr;

IMPLEMENT_SINGLETON(CGameInstance)



CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(HWND windowHandle, CRawInput_Device* InputDev)
{
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

#pragma region Create
	//-----------------------------Input Dev-----------------------------
	m_Input_Dev = InputDev;
	Safe_AddRef(m_Input_Dev);


	//---------------------------Graphic_Dev------------------------

	m_pGraphic_Device = CGraphic_Device::Create(windowHandle);
	if(nullptr == m_pGraphic_Device)
		return E_FAIL;

	ThrowIfFailed(m_pGraphic_Device->Get_CommandList()->Reset(m_pGraphic_Device->Get_CommandAlloc(), nullptr));

	//---------------------------ComMgr------------------------
	m_ComMgr = CComponentMgr::Get_Instance();
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	/*m_ComMgr->AddPrototype("TransformCom", CTransform::Create());
	m_ComMgr->AddPrototype("BaseGeoCom", CVIBuffer_Geos::Create(Get_Device(), Get_CommandList()));
	m_ComMgr->AddPrototype("TerrainCom", CVIBuffer_Terrain::Create(Get_Device(), Get_CommandList(), "../Bin/Models/HeightMap/Height.png", 0.1f, 1.f));*/


	//-------------------------Root Signature---------------------------
	Build_RootSignatures();

	
	//------------------------ShaderMgr-------------------------------------------
	Build_Shaders();
	

	//---------------------PSOMgr---------------------------------------------------
	Build_PSOs();


	//---------------------FrameResourceMgr-------------------------------------------------
	Build_FrameResources();

	////----------------------------Renderer
	Create_Renderer();

	//-------------------------------------ObjMgr-------------------------------------

	m_ObjMgr = CObjectMgr::Get_Instance();

	//--------------------------------------------------------------------------------------

	m_TextureMgr = CTextureMgr::Get_Instance();

	//---------------------------------------------------------------

	m_MaterialMgr = CMaterialMgr::Get_Instance();


#pragma endregion

	ThrowIfFailed(m_pGraphic_Device->Get_CommandList()->Close());
	ID3D12CommandList* cmdsLists[] = { m_pGraphic_Device->Get_CommandList() };
	m_pGraphic_Device->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	m_pGraphic_Device->FlushCommandQueue();

	return S_OK;
}

#pragma region GraphicDev

ID3D12Device* CGameInstance::Get_Device() const
{
	 return m_pGraphic_Device->Get_Device(); 
}

ID3D12GraphicsCommandList* CGameInstance::Get_CommandList() const
{
	return m_pGraphic_Device->Get_CommandList(); 
}

ID3D12CommandQueue* CGameInstance::Get_CommandQueue() const
{
	return m_pGraphic_Device->Get_CommandQueue(); 
}

ID3D12CommandAllocator* CGameInstance::Get_CommandAlloc() const
{
	return m_pGraphic_Device->Get_CommandAlloc(); 
}

ID3D12Fence* CGameInstance::Get_Fence() const
{
	return m_pGraphic_Device->Get_Fence();
}

UINT64& CGameInstance::Get_Fence_Value()
{
	return m_pGraphic_Device->Get_Fence_Value();
}

void CGameInstance::Draw_1()
{
	m_pGraphic_Device->Draw_1();
}

void CGameInstance::Draw_2()
{
	m_pGraphic_Device->Draw_2();
}

#pragma endregion

//------------------For ComMgr
HRESULT CGameInstance::AddPrototype(const string& prototypeTag, CComponent* pPrototype)
{
	if(FAILED(m_ComMgr->AddPrototype(prototypeTag, pPrototype)))
		return E_FAIL;
	return S_OK;
}

CComponent* CGameInstance::GetComponent(const string& prototypeTag, void* pArg) const
{
	return m_ComMgr->GetComponent(prototypeTag, pArg);
}

//----------------------------------For ShaderMgr---------------------------------------------

CShader* CGameInstance::GetShaderObj(const string& shaderName) const
{
	return m_ShaderMgr->GetShaderObj(shaderName);
}

BYTE* CGameInstance::GetShader(const string& shaderName) const
{
	return m_ShaderMgr->GetShader(shaderName);
}

SIZE_T CGameInstance::GetBufferSize(const string& shaderName) const
{
	return m_ShaderMgr->GetBufferSize(shaderName);
}

//-------------------------For PSOMgr----------------------------------------------------------

HRESULT CGameInstance::AddPSO(const string& PSOName, ID3D12RootSignature* pRootSignature, CShader* VS, CShader* PS, CGraphic_Device* pGD, ID3D12Device* pDevice, CPSO::INPUTLAYOUT_TYPE eLayout)
{
	return m_PSOMgr->AddPSO(PSOName,pRootSignature,VS,PS,pGD,pDevice,eLayout);
}

CPSO* CGameInstance::GetPSOObj(const string& PSOName) const
{
	return m_PSOMgr->GetPSOObj(PSOName);
}

ID3D12PipelineState* CGameInstance::Get_PSO(const string& PSOName) const
{
	return m_PSOMgr->Get(PSOName);
}

//----------------For ObjMgr----------------------------------------------------

HRESULT CGameInstance::AddObject(const string& layerTag, CGameObject* pGameObject)
{
	return m_ObjMgr->AddObject(layerTag,pGameObject);
}

//-------------------------For InputDev-----------------------------------------

bool CGameInstance::Key_Down(UINT vkey)
{
	return m_Input_Dev->Key_Down(vkey);
}

bool CGameInstance::Key_Up(UINT vkey)
{
	return m_Input_Dev->Key_Up(vkey);
}

bool CGameInstance::Key_Pressing(UINT vkey)
{
	return m_Input_Dev->Key_Pressing(vkey);
}

bool CGameInstance::Mouse_Down(MOUSEKEYSTATE button)
{
	return m_Input_Dev->Mouse_Down(button);
}

bool CGameInstance::Mouse_Up(MOUSEKEYSTATE button)
{
	return m_Input_Dev->Mouse_Up(button);
}

bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE button)
{
	return m_Input_Dev->Mouse_Pressing(button);
}


//----------------------------------Builds---------------------------------------------




void CGameInstance::Build_RootSignatures()
{
	m_RootSignatureMgr = CRootSignatureMgr::Get_Instance();

	CRootSignatureBuilder builder1;
	builder1
		.Push(RootParam::ObjectCB, D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_ALL) // slot 0
		.Push(RootParam::PassCB, D3D12_ROOT_PARAMETER_TYPE_CBV, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // slot 1
		//.Push(RootParam::MaterialBuffer, D3D12_ROOT_PARAMETER_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_ALL) // slot 2
		//.PushTable(RootParam::Textures, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // slot 3

	m_RootSignatureMgr->Register("DefaultRS", Get_Device(), builder1);

	//m_RootSignatureMgr = CRootSignatureMgr::Get_Instance();

	//CRootSignatureBuilder builder1;
	//builder1
	//	.Push(RootParam::ObjectCB, D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0, D3D12_SHADER_VISIBILITY_ALL) // slot 0
	//	.Push(RootParam::PassCB, D3D12_ROOT_PARAMETER_TYPE_CBV, 1, 0, D3D12_SHADER_VISIBILITY_ALL) // slot 1
	//	.Push(RootParam::MaterialBuffer, D3D12_ROOT_PARAMETER_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_ALL) // slot 2
	//	.PushTable(RootParam::Textures, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // slot 3

	//m_RootSignatureMgr->Register("DefaultRS", Get_Device(), builder1);


}

void CGameInstance::Build_Shaders()
{
	m_ShaderMgr = CShader_Mgr::Get_Instance();

	m_ShaderMgr->AddShader("DefaultVS", CShader::ST_VS, L"../Bin/Shader/Default.hlsl", nullptr);
	m_ShaderMgr->AddShader("DefaultPS", CShader::ST_PS, L"../Bin/Shader/Default.hlsl", nullptr);

	//m_ShaderMgr->AddShader("TerrainVS", CShader::ST_VS, L"../Bin/Shaders/Terrain.hlsl", nullptr);
	//m_ShaderMgr->AddShader("TerrainPS", CShader::ST_PS, L"../Bin/Shaders/Terrain.hlsl", nullptr);
}

void CGameInstance::Build_PSOs()
{
	m_PSOMgr = CPSOMgr::Get_Instance();
	m_PSOMgr->AddPSO("DefaultPSO", m_RootSignatureMgr->Get("DefaultRS"), 
		m_ShaderMgr->GetShaderObj("DefaultVS"), m_ShaderMgr->GetShaderObj("DefaultPS"),
		m_pGraphic_Device, Get_Device(), CPSO::IT_POS_NOR);

	//m_PSOMgr->AddPSO("TerrainPSO", m_RootSignatureMgr->Get("TerrainRS"),
	//	m_ShaderMgr->GetShaderObj("TerrainVS"), m_ShaderMgr->GetShaderObj("TerrainPS"),
	//	m_pGraphic_Device, Get_Device(), CPSO::IT_POS_TEX);
}

void CGameInstance::Build_FrameResources()
{
	m_FrameResourceMgr = CFrameResourceMgr::Get_Instance();
	m_FrameResourceMgr->Initialize(Get_Device(), gNumFrameResources);
}

void CGameInstance::Create_Renderer()
{
	m_MainRenderer = CRenderer::Create(Get_Device(),Get_CommandList() );
	m_ComMgr->AddPrototype("RendererCom", m_MainRenderer);
}


//HRESULT CGameInstance::RemoveObject(const string& layerTag, CGameObject* pGameObject)
//{
//	return E_NOTIMPL;
//}

void CGameInstance::Update(const CTimer* gameTimer)
{
	//m_Input_Dev->Update_InputDev();

	AdvanceFrame();
	auto CurrFrameResource = GetCurrentFrameResource();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (CurrFrameResource->m_Fence != 0 && m_pGraphic_Device->Get_Fence()->GetCompletedValue() < CurrFrameResource->m_Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_pGraphic_Device->Get_Fence()->SetEventOnCompletion(CurrFrameResource->m_Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	const float fTimeDelta = gameTimer->DeltaTime();

	m_ObjMgr->Update(fTimeDelta);
}

void CGameInstance::Late_Update(const CTimer* gameTimer)
{
	const float fTimeDelta = gameTimer->DeltaTime();
	m_ObjMgr->LateUpdate(fTimeDelta);
}

void CGameInstance::Draw()
{
	if(!m_MainRenderer)
		return;

	auto cmdListAlloc = GetCurrentFrameResource()->m_CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(m_pGraphic_Device->Get_CommandList()->Reset(cmdListAlloc, Get_PSO("DefaultPSO")));

	Draw_1();

	m_pGraphic_Device->Get_CommandList()->SetGraphicsRootSignature(m_RootSignatureMgr->Get("DefaultRS"));

	// Bind per-pass constant buffer.  We only need to do this once per-pass.
	auto passCB = GetCurrentFrameResource()->m_PassCB->Resource();
	m_pGraphic_Device->Get_CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());



	m_MainRenderer->Render();

	m_pGraphic_Device->Draw_2();

	GetCurrentFrameResource()->m_Fence = ++m_pGraphic_Device->Get_Fence_Value();

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	m_pGraphic_Device->Get_CommandQueue()->Signal(m_pGraphic_Device->Get_Fence(), m_pGraphic_Device->Get_Fence_Value());

	m_MainRenderer->ResetRenderObjects();
}

void CGameInstance::OnResize()
{
	if (!m_pGraphic_Device)
		return;

	m_pGraphic_Device->OnResize();
}

void CGameInstance::Free()
{
	//m_pGraphic_Device->FlushCommandQueue();

	m_pGraphic_Device->Get_CommandQueue()->Wait(Get_Fence(), m_FrameResourceMgr->Get_Final_Fence());

	UINT64 a = m_pGraphic_Device->Get_Fence_Value();
	UINT64 b = m_FrameResourceMgr->Get_Final_Fence();

	Safe_Release(m_Input_Dev);
	Safe_Release(m_RootSignatureMgr);
	Safe_Release(m_ShaderMgr);
	Safe_Release(m_PSOMgr);
	Safe_Release(m_FrameResourceMgr);
	Safe_Release(m_MainRenderer);
	Safe_Release(m_ObjMgr);
	Safe_Release(m_ComMgr);
	Safe_Release(m_pGraphic_Device);
}

