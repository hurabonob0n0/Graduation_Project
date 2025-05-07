#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTimer;
//class CInput_Device;
class CRawInput_Device;
END

BEGIN(Client)

class CMainApp : public CBase
{
	DECLARE_SINGLETON(CMainApp)
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	int Run();

public:
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT Initialize_MainWindow(HINSTANCE g_hInstance);
	HRESULT Initialize(HINSTANCE g_hInstance);

	void Prepare_Textures();
	void Prepare_Materials();
	void Prepare_Components();
	void Make_Objects();

public:
	void Update(const CTimer* Timer);
	void Late_Update(const CTimer* Timer);

public:
	void Draw();

public: //----------- For Debug : FPS ---------------
	void CalculateFrameStats();


private:
	HINSTANCE	m_hAppInst = nullptr;
	HWND		m_hMainWnd = nullptr;

private:
	bool		m_AppPaused = false; 
	bool		m_Minimized = false;
	bool		m_Maximized = false;
	bool		m_Resizing = false;
	bool		m_FullscreenState = false;

private:
	int							m_ClientWidth = 2560;
	int							m_ClientHeight = 1440;
	std::wstring                m_MainWndCaption = L"Two the Tanks";

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	//CInput_Device*		m_pInput_Dev{ nullptr };
	CRawInput_Device* m_pInput_Dev{ nullptr };
	CTimer*				m_pTimer = { nullptr };

public:
	virtual void Free() override;


};


END