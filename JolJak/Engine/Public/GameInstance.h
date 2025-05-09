#pragma once
#include "Base.h"
#include "Graphic_Device.h"
#include "ComponentMgr.h"
#include "Shader_Mgr.h"
#include "RootSignatureMgr.h"
#include "PSOMgr.h"
#include "FrameResourceMgr.h"
#include "ObjectMgr.h"
#include "Raw_Input.h"
#include "Input_Device.h"
#include "TextureMgr.h"
#include "MaterialMgr.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	void OnResize();

public:
	HRESULT Initialize_Engine(HWND windowHandle, CRawInput_Device* InputDev);

public:			// For Graphic_Dev
	ID3D12Device* Get_Device() const; 
	ID3D12GraphicsCommandList* Get_CommandList() const; 
	ID3D12CommandQueue* Get_CommandQueue() const; 
	ID3D12CommandAllocator* Get_CommandAlloc() const; 
	ID3D12Fence* Get_Fence() const; 
	UINT64& Get_Fence_Value();
	void Draw_1();
	void Draw_2();


public:			// For ComMgr
	HRESULT					AddPrototype(const string& prototypeTag, CComponent* pPrototype);
	CComponent*				GetComponent(const string& prototypeTag, void* pArg = nullptr) const;
	CComponent*				GetPrototype(const string& prototypeTag) { return m_ComMgr->GetPrototype(prototypeTag); }


public:			// For RootSignature
	ID3D12RootSignature*	Get_RootSignature(string RSname) const { return m_RootSignatureMgr->Get(RSname); }


public:			// For ShaderMgr
	CShader*				GetShaderObj(const string& shaderName) const;
	BYTE*					GetShader(const string& shaderName) const;
	SIZE_T					GetBufferSize(const string& shaderName) const;

public:			// For PSOMgr
	HRESULT					AddPSO(const string& PSOName, ID3D12RootSignature* pRootSignature, CShader* VS, CShader* PS, CGraphic_Device* pGD, ID3D12Device* pDevice, CPSO::INPUTLAYOUT_TYPE eLayout);
	CPSO*					GetPSOObj(const string& PSOName) const;
	ID3D12PipelineState*	Get_PSO(const string& PSOName) const;

public:			// For FrameResourceMgr
	CFrameResource*			GetCurrentFrameResource() const { return m_FrameResourceMgr->GetCurrentFrameResource(); };
	UINT					GetCurrentFrameIndex() const { return m_FrameResourceMgr->GetCurrentFrameIndex(); }
	void					AdvanceFrame() { m_FrameResourceMgr->AdvanceFrame(); };

public:			// For ObjMgr
	HRESULT AddObject(const string& layerTag, CGameObject* pGameObject);
	void Set_Pos_For_Server(string LayerName, _uint ID, float* pos) { m_ObjMgr->Set_Pos_For_Server(LayerName, ID, pos); }
	//HRESULT RemoveObject(const string& layerTag, CGameObject* pGameObject);


public:			// For InputMgr
	bool Key_Down(UINT vkey);
	bool Key_Up(UINT vkey);
	bool Key_Pressing(UINT vkey);

	bool Mouse_Down(MOUSEKEYSTATE button);
	bool Mouse_Up(MOUSEKEYSTATE button);
	bool Mouse_Pressing(MOUSEKEYSTATE button);

	SHORT Get_Mouse_XDelta() { return m_Input_Dev->Get_Mouse_XDelta(); }
	SHORT Get_Mouse_YDelta() { return m_Input_Dev->Get_Mouse_YDelta(); }

public:			// For TextureMgr
	void Add_Texture(string texname, CTexture* texInstance) { m_TextureMgr->Add_Texture(texname, texInstance); }
	ID3D12Resource* Get_Texture(string texname) { return m_TextureMgr->Get_Texture(texname); }
	_uint Get_Tex_Index(string texname) { return m_TextureMgr->Get_Tex_Index(texname); }

public:			// For MatMgr
	void Add_Material(string matName, MaterialConstants* matInstance) { m_MaterialMgr->Add_Material(matName, matInstance); }
	_uint Get_Mat_Index(string matName) { return m_MaterialMgr->Get_Mat_Index(matName); }

public:
	//const float& Get_Terrain_Height() { return (CVIBuffer_Terrain*)m_ComMgr->GetPrototype("VIBuffer_Terrain")->}



public:
	void Build_RootSignatures();
	void Build_Shaders();
	void Build_PSOs();
	void Build_FrameResources();
	void Create_Renderer();


public:
	void Update(const class CTimer* gameTimer);
	void Late_Update(const class CTimer* gameTimer);
	void Draw();

public:
	class CGraphic_Device*			m_pGraphic_Device;

private:
	CRootSignatureMgr*				m_RootSignatureMgr;
	CShader_Mgr*					m_ShaderMgr;
	CPSOMgr*						m_PSOMgr;
	CFrameResourceMgr*				m_FrameResourceMgr;
	CRenderer*						m_MainRenderer;
	CObjectMgr*						m_ObjMgr;
	CComponentMgr*					m_ComMgr;
	CRawInput_Device*				m_Input_Dev;
	CTextureMgr*					m_TextureMgr;
	CMaterialMgr*					m_MaterialMgr;

private:
	//vector<class CGameObject*>		m_vGameObjects;
	

public:
	virtual void Free() override;
};

END