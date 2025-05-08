#include "pch.h"
/*-----------------
	For Client
-----------------*/
#include "CGlobals.h"
#include <windowsx.h>
#include "MainApp.h"
#include "GameInstance.h"
#include "Timer.h"
#include "Raw_Input.h"
#include "BoxObj.h"
#include "Terrain.h"
#include "Input_Device.h"
#include "Tank.h"

/*-----------------
	For Server
-----------------*/
#include "ThreadManager.h"
//#include "Service.h"//�̵� ������ ��
#include "Session.h"
#include "BufferReader.h"
#include "ClientPacketHandler.h"
#include "ServiceManager.h"


static int MYClientID;


IMPLEMENT_SINGLETON(CMainApp)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd){
	
	//_CrtSetBreakAlloc(624);
	//_CrtSetBreakAlloc(622);
	//_CrtSetBreakAlloc(623);
	//_CrtSetBreakAlloc(720);
	//_CrtSetBreakAlloc(704);
	//_CrtSetBreakAlloc(706);
	//_CrtSetBreakAlloc(623);
	//_CrtSetBreakAlloc(624);
	//_CrtSetBreakAlloc(622);
	//_CrtSetBreakAlloc(623);
	//_CrtSetBreakAlloc(159);
	//_CrtSetBreakAlloc(159);
	//_CrtSetBreakAlloc(158);
	//_CrtSetBreakAlloc(159);
	//_CrtSetBreakAlloc(159);
	try
	{
		CMainApp* mainApp = CMainApp::Get_Instance();
		if (FAILED(mainApp->Initialize(hInstance)))
			return 0;

		return mainApp->Run();
	}
	catch (DxException& e)
	{
		return 0;
	}

}


LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// CreateWindow�� �Ϸ�Ǳ� ������ mhMainWnd�� ��ȿ���� �ʽ��ϴ�.
	// ������ CreateWindow�� �Ϸ�Ǳ� ���� �޼��� (���� ��� WM_CREATE)��
	// ���� �� �ֱ� ������ hwnd�� �����մϴ�.
	return CMainApp::Get_Instance()->MsgProc(hwnd, msg, wParam, lParam);
}

CMainApp::CMainApp() : m_GameInstance(CGameInstance::Get_Instance()), m_pTimer(CTimer::Get_Instance()), m_pInput_Dev(CRawInput_Device::Get_Instance()) //m_pInput_Dev(CInput_Device::Get_Instance())
{
	//ShowCursor(FALSE);                   // Ŀ�� �����
	ClipCursor(nullptr);                 // Ŭ���� ���� (�ٸ� ���� NULL ������)
}

int CMainApp::Run()
{
	MSG msg = { 0 };

	m_pTimer->Reset();
	const float targetTime = 1.0f / 30.0f;


	while (msg.message != WM_QUIT)
	{
		// ó���ؾ��� ������ �޼������� �ִ��� Ȯ���մϴ�.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ó���ؾ��� �޼����� ���� ���, ���ϸ��̼ǰ� ������ ó���մϴ�.
		else
		{
			m_pTimer->Tick();

			if (!m_AppPaused)
			{
				
				CalculateFrameStats();
				Update(m_pTimer);




				Late_Update(m_pTimer);
				Draw();
				
				

				// 3) Ŀ���� ������ ������ ������ �ʵ��� Ŭ����
				//ClipCursor(&rc);

				
				//m_pInput_Dev->UpdateKeyStates();
				//m_pInput_Dev->ResetPerFrame();

				//POINT center = {
				//	1280,720
				//};
				//ClientToScreen(m_hMainWnd, &center);

				//// 2) ���콺 Ŀ�� �߾� �̵�
				//SetCursorPos(center.x, center.y);



				float frameTime = m_pTimer->DeltaTime();
				if (frameTime < targetTime)
				{
					// �и��ʷ� ���, ��ǥ �ð����� ������ �κ��� �����
					Sleep(static_cast<DWORD>((targetTime - frameTime) * 1000.0f));
				}
			}
			else
			{
				Sleep(100);
			}
		}
	}

	RELEASE_INSTANCE(CMainApp);

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return (int)msg.wParam;
}

LRESULT CMainApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE�� �����찡 Ȱ��ȭ �ǰų� ��Ȱ��ȭ �ɶ� �������ϴ�.
		// �����찡 ��Ȱ��ȭ �Ǿ��� ���� ������ ������Ű��
		// �ٽ� Ȱ��ȭ �Ǿ��� ���� ������ �ٽ� ����մϴ�.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_pTimer->Stop();
		}
		else
		{
			m_AppPaused = false;
			m_pTimer->Start();
		}
		return 0;

		// WM_SIZE�� ����ڰ� ������ ũ�⸦ ������ �� �������ϴ�.
	case WM_SIZE:
		// ���ο� ������ ũ�⸦ �����մϴ�.
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);

		if (m_GameInstance)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				//OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// �ּ�ȭ ���·κ��� �ٽ� �����Ǵ� �̴ϱ�?
				if (m_Minimized)
				{
					m_AppPaused = false;
					m_Minimized = false;
					//OnResize();
				}
				// �ִ�ȭ ���·κ��� �ٽ� �����Ǵ� �̴ϱ�?
				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;
					//OnResize();
				}
				else if (m_Resizing)
				{
					// ����ڰ� ũ�� ���� �ٸ� �巡���ϴ� ��츶�� �Ź� �������� ó���� ���ָ�
					// ������ ũ�� �϶��ϰ� �˴ϴ�. �׷��Ƿ� �Ź� �������� ó���� ���� �ʰ�
					// ����ڰ� ũ�� ������ ������ �� �߻��ϴ� WM_EXITSIZEMOVE���� �������� ó���� �մϴ�.
				}
				else // SetWindowPos �Ǵ� mSwapChain->SetFullscreenState�� ���ؼ� API�� ȣ�����ϴ�.
				{
					//OnResize();
				}
			}
		}
		return 0;

		// ����ڰ� ũ�� ������ �������� �� WM_EXITSIZEMOVE�� �������ϴ�.
	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing = true;
		m_pTimer->Stop();
		return 0;

		// ����ڰ� ũ�� ������ �������� �� WM_EXITSIZEMOVE�� �������ϴ�.
		// ���⼭ ���ο� ������ ũ�⿡ ���� ó���� ��� �մϴ�.
	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing = false;
		m_pTimer->Start();
		//OnResize();
		return 0;

		// �����찡 �ı��� �� WM_DESTORY�� �������ϴ�.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// WM_MENUCAR�� �޴��� Ȱ��ȭ �ɶ� �������ϴ�.
		// ����ڰ� �ϸ��, ���ӱ� Ű�� �ش����� �ʴ� Ű�� �����ϴ�.
	case WM_MENUCHAR:
		// ��Ʈ + ���͸� �Է½� �������� �߻��ϴ°��� �����մϴ�.
		return MAKELRESULT(0, MNC_CLOSE);

		// �����찡 �ʹ� �۾����°��� �����մϴ�.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		/*case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;*/

	case WM_INPUT:
		m_pInput_Dev->Update_InputDev(lParam);
		break;

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		/*else if ((int)wParam == VK_F2)
		{
			Set4xMsaaState(!m4xMsaaState); // �� ���� ��Ƽ�ٸ���� �񽺹����� �� Ű�� ��.
		}*/
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HRESULT CMainApp::Initialize_MainWindow(HINSTANCE g_hInstance)
{
	m_hAppInst = g_hInstance;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Ŭ���̾�Ʈ�� ũ�⸦ ������� ������ �簢���� ����մϴ�.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"MainWnd", m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

#pragma region For ServerSession Class (Work by Packet)
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
	}

	virtual void OnConnected() override
	{
		g_ServerConnected.store(true);
		SendBufferRef sendBuffer = ClientPacketHandler::Make_C_LOGIN(1001, 100, 10);
		Send(sendBuffer);
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		ClientPacketHandler::HandlePacket(buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
	}

	virtual void OnDisconnected() override
	{
	}
};
#pragma endregion Dont Touch!

HRESULT CMainApp::Initialize(HINSTANCE g_hInstance)
{

#pragma region ServerInitConnect

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	ASSERT_CRASH(service->Start());

	ServiceManager::GetInstace().SetService(service);

	int32 threadCount = std::thread::hardware_concurrency();
	for (int32 i = 0; i < threadCount; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

#pragma endregion Dont Touch!


#pragma region Base Initialize
	

	
	Initialize_MainWindow(g_hInstance);

	//m_pInput_Dev->Initialize(g_hInstance,m_hMainWnd);
	m_pInput_Dev->Initialize(m_hMainWnd);

	if (FAILED(m_GameInstance->Initialize_Engine(m_hMainWnd,m_pInput_Dev)))
		return E_FAIL;

	m_GameInstance->OnResize();
#pragma endregion

	m_GameInstance->m_pGraphic_Device->FlushCommandQueue();

	m_GameInstance->m_pGraphic_Device->Get_CommandList()->Reset(m_GameInstance->Get_CommandAlloc() , nullptr);

	m_GameInstance->AddPrototype("TransformCom", CTransform::Create());
	//m_pGameInstance->AddPrototype("TerrainCom", CVIBuffer_Terrain::Create(m_pGameInstance->Get_Device(),m_pGameInstance->Get_CommandList(),"../Bin/Models/Terrain/Terrain.png",0.3f,1.f));
	m_GameInstance->AddPrototype("BaseGeosCom", CVIBuffer_Geos::Create(m_GameInstance->Get_Device(), m_GameInstance->Get_CommandList()));
	//m_GameInstance->AddPrototype("TankModel", CModel::Create(m_GameInstance->Get_Device(), m_GameInstance->Get_CommandList(), CModel::TYPE_NONANIM, "../bin/Models/Tank/M1A2.FBX"));

	//CModel* pModel = CModel::Create(m_pGameInstance->Get_Device(), m_pGameInstance->Get_CommandList(), CModel::TYPE_NONANIM, "../bin/Models/Tank/M1A2.FBX");

	//m_GameInstance->AddObject("Camera", CCamera::Create());


#pragma region ���� ��ġ����ȭ �׽�Ʈ�� �ӽ� �ڽ� ��ü ���� + �α��� �� �Ǿ�� �ϱ⿡ ���� �ڵ�



#pragma endregion ForTest

	//m_pGameInstance->AddObject("Terrain", CTerrain::Create());
	//m_pGameInstance->AddObject("Tank", CTank::Create());

	m_GameInstance->Get_CommandList()->Close();
	ID3D12CommandList* cmdLists[] = { m_GameInstance->Get_CommandList() };
	m_GameInstance->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// ũ�� ������ ���� ��ɵ��� ó���ɶ����� ��ٸ���.
	m_GameInstance->m_pGraphic_Device->FlushCommandQueue();



	while (!g_ServerConnected.load()) {

	}

	CBoxObj* Box0 = CBoxObj::Create();
	CBoxObj* Box1 = CBoxObj::Create();

	if (g_PlayerID.load() == 0) {
		Box0->set_MyPlayer();
	}
	else {
		Box1->set_MyPlayer();
	}

	m_GameInstance->AddObject("BoxObj", Box0);
	m_GameInstance->AddObject("BoxObj", Box1);

	float pos0[3] = { 0.f,0.f,0.f };
	float pos1[3] = { 10.f,0.f,0.f };

	m_GameInstance->Set_Pos_For_Server("BoxObj", 0, pos0);
	m_GameInstance->Set_Pos_For_Server("BoxObj", 1, pos1);

	return S_OK;
}

void CMainApp::Prepare_Textures()
{
	std::vector<std::string> texNames =
	{
		"TerrainDiffuse",
		"TerrainNormal",
		"",
		"tileNormalMap",
		"defaultDiffuseMap",
		"defaultNormalMap",
		"skyCubeMap"
	};

	std::vector<std::wstring> texFilenames =
	{
		L"../../Textures/bricks2.dds",
		L"../../Textures/bricks2_nmap.dds",
		L"../../Textures/tile.dds",
		L"../../Textures/tile_nmap.dds",
		L"../../Textures/white1x1.dds",
		L"../../Textures/default_nmap.dds",
		L"../../Textures/snowcube1024.dds"
	};
}

void CMainApp::Prepare_Materials()
{
}

void CMainApp::Prepare_Components()
{
}

void CMainApp::Make_Objects()
{
}

void CMainApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	++frameCnt;

	// 1�� ������ ����� ����մϴ�.
	if ((m_pTimer->TotalTime() - timeElapsed >= 1.0f))
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1;
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mpsfStr = to_wstring(mspf);
		wstring	MyID = to_wstring(g_PlayerID.load());

		wstring OtherPlayerPosX = to_wstring(otherPosX);
		wstring OtherPlayerPosY = to_wstring(otherPosY);
		wstring OtherPlayerPosZ = to_wstring(otherPosZ);


		wstring windowText = m_MainWndCaption +
			L"    fps: " + fpsStr +
			L"   mfps: " + mpsfStr +
			L"   CLIENT ID : " + MyID +
			L"	OtherPos" + OtherPlayerPosX +
			L"  " + OtherPlayerPosY + L"  " + OtherPlayerPosZ;


		SetWindowText(m_hMainWnd, windowText.c_str());

		// ���� ����� ���� �ʱ�ȭ �մϴ�.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void CMainApp::Update(const CTimer* Timer)
{
	m_GameInstance->Update(m_pTimer);
}

void CMainApp::Late_Update(const CTimer* Timer)
{
	m_GameInstance->Late_Update(Timer);
}

void CMainApp::Draw()
{
	m_GameInstance->Draw();
}

void CMainApp::Free()
{
	Safe_Release(m_pInput_Dev);
	Safe_Release(m_pTimer);
	Safe_Release(m_GameInstance);
}
