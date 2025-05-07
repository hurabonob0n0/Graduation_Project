#include "Raw_Input.h"

IMPLEMENT_SINGLETON(CRawInput_Device)

inline CRawInput_Device::CRawInput_Device() {}

HRESULT CRawInput_Device::Initialize(HWND hWnd)
{
	RAWINPUTDEVICE Rid[2] = {};

	// Ű����
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x06;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hWnd;

	// ���콺
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x02;
	Rid[1].dwFlags = RIDEV_INPUTSINK;
	Rid[1].hwndTarget = hWnd;

	if (!RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)))
		return E_FAIL;

	return S_OK;
}

void CRawInput_Device::Update_InputDev(LPARAM lParam)
{
	UINT dwSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
	std::vector<BYTE> lpb(dwSize);
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		return;

	RAWINPUT* raw = (RAWINPUT*)lpb.data();

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		UINT vkey = raw->data.keyboard.VKey;
		bool isDown = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
		if (isDown)
		{
			m_KeyState[vkey] |= KEY_DOWN;   // KeyDown�� ON
			m_KeyState[vkey] &= ~KEY_UP;    // KeyUp OFF
		}
		else
		{
			m_KeyState[vkey] |= KEY_UP;     // KeyUp ON
			m_KeyState[vkey] &= ~KEY_DOWN;  // KeyDown OFF
		}
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		RAWMOUSE& mouse = raw->data.mouse;

		m_MouseDelta[0] = static_cast<SHORT>(mouse.lLastX);
		m_MouseDelta[1] = static_cast<SHORT>(mouse.lLastY);

		if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
			m_bMouseCurrState[0] = true;
		if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
			m_bMouseCurrState[0] = false;

		if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			m_bMouseCurrState[1] = true;
		if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
			m_bMouseCurrState[1] = false;

		if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			m_bMouseCurrState[2] = true;
		if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
			m_bMouseCurrState[2] = false;

		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			m_bMouseCurrState[3] = true;
		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			m_bMouseCurrState[3] = false;

		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			m_bMouseCurrState[4] = true;
		if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			m_bMouseCurrState[4] = false;

		if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
			m_MouseDelta[2] = static_cast<SHORT>(mouse.usButtonData);
		else
			m_MouseDelta[2] = 0;
	}
}

bool CRawInput_Device::Key_Down(UINT vkey)
{
	/*if (!m_bKeyPrevState[vkey] && (m_KeyState[vkey] & 0x80))
	{
		m_bKeyPrevState[vkey] = true;
		return true;
	}
	if (m_bKeyPrevState[vkey] && !(m_KeyState[vkey] & 0x80))
		m_bKeyPrevState[vkey] = false;*/

	//return (m_KeyState[vkey] & KEY_PRESSING) != 0;


	//return false;

	return !m_bKeyPrevState[vkey] && (m_KeyState[vkey] & KEY_DOWN);
}

bool CRawInput_Device::Key_Up(UINT vkey)
{
	/*if (m_bKeyPrevState[vkey] && !(m_KeyState[vkey] & 0x80))
	{
		m_bKeyPrevState[vkey] = false;
		return true;
	}
	if (!m_bKeyPrevState[vkey] && (m_KeyState[vkey] & 0x80))
		m_bKeyPrevState[vkey] = true;

	return false;*/
	return m_bKeyPrevState[vkey] && (m_KeyState[vkey] & KEY_UP);

	
}

bool CRawInput_Device::Key_Pressing(UINT vkey)
{
	//return (m_KeyState[vkey] & 0x80) != 0;

	return m_bKeyPrevState[vkey] || (m_KeyState[vkey] & KEY_DOWN);
}

bool CRawInput_Device::Mouse_Down(UINT button)
{
	if (!m_bMousePrevState[button] && m_bMouseCurrState[button])
	{
		m_bMousePrevState[button] = true;
		return true;
	}
	if (m_bMousePrevState[button] && !m_bMouseCurrState[button])
		m_bMousePrevState[button] = false;

	return false;
}

bool CRawInput_Device::Mouse_Up(UINT button)
{
	if (m_bMousePrevState[button] && !m_bMouseCurrState[button])
	{
		m_bMousePrevState[button] = false;
		return true;
	}
	if (!m_bMousePrevState[button] && m_bMouseCurrState[button])
		m_bMousePrevState[button] = true;

	return false;
}

bool CRawInput_Device::Mouse_Pressing(UINT button)
{
	return m_bMouseCurrState[button];
}

void CRawInput_Device::ResetPerFrame()
{
	// Mouse delta �ʱ�ȭ
	m_MouseDelta[0] = 0;
	m_MouseDelta[1] = 0;
	m_MouseDelta[2] = 0;
}

void CRawInput_Device::UpdateKeyStates()
{
	for (int i = 0; i < 256; ++i)
	{
		m_bKeyPrevState[i] = (m_KeyState[i] & KEY_DOWN || m_KeyState[i] & KEY_PRESSING);
	}
}

void CRawInput_Device::Free()
{
}
