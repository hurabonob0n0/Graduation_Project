#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	CInput_Device();	
	virtual ~CInput_Device(void) = default;
	
public:
	BYTE	Get_DIKeyState(unsigned char byKeyID) const {
		return m_byKeyState[byKeyID]; 
	}

	BYTE	Get_DIMouseState(MOUSEKEYSTATE eMouse) const { return m_tMouseState.rgbButtons[eMouse]; 	}
	LONG	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	const
	{	
		return *(((LONG*)&m_tMouseState) + eMouseState);
	}
public:
	BOOL Key_Down(unsigned char byKeyID);
	BOOL Mouse_Down(MOUSEKEYSTATE MKS);

	BOOL Key_Pressing(unsigned char byKeyID);
	BOOL Mouse_Pressing(MOUSEKEYSTATE MKS);

	BOOL Key_Up(unsigned char byKeyID);
	BOOL Mouse_Up(MOUSEKEYSTATE MKS);

	SHORT Get_Mouse_XDelta() { return Get_DIMouseMove(MOUSEMOVESTATE::X); }
	SHORT Get_Mouse_YDelta() { return Get_DIMouseMove(MOUSEMOVESTATE::Y); }
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void); /* 키보드와 마우스의 상태를 저장한다. */

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	BYTE					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState;	

private:
	BOOL			m_bKeyState[256] = { false };
	BOOL			m_bMouseState[MOUSEKEYSTATE::MKS_END] = { false };

public:
	//static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
