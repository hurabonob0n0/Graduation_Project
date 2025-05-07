#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{

}

BOOL CInput_Device::Key_Down(unsigned char byKeyID)
{
	if (!m_bKeyState[byKeyID] && (Get_DIKeyState(byKeyID) & 0x80))
	{
		m_bKeyState[byKeyID] = !m_bKeyState[byKeyID];
		return true;
	}
	if (m_bKeyState[byKeyID] && !(Get_DIKeyState(byKeyID) & 0x80))
		m_bKeyState[byKeyID] = !m_bKeyState[byKeyID];

	return false;

}

BOOL CInput_Device::Mouse_Down(MOUSEKEYSTATE MKS)
{
	if (!m_bMouseState[MKS] && (Get_DIMouseState(MKS) & 0x80))
	{
		m_bMouseState[MKS] = !m_bMouseState[MKS];
		return true;
	}
	if (m_bMouseState[MKS] && !(Get_DIMouseState(MKS) & 0x80))
		m_bMouseState[MKS] = !m_bMouseState[MKS];

	return false;
}

BOOL CInput_Device::Key_Pressing(unsigned char byKeyID)
{
	if (Get_DIKeyState(byKeyID) & 0x80)
		return true;

	m_bKeyState[byKeyID] = true;

	return false;
}

BOOL CInput_Device::Mouse_Pressing(MOUSEKEYSTATE MKS)
{
	if (Get_DIMouseState(MKS) & 0x80)
		return true;

	m_bMouseState[MKS] = true;

	return false;
}

BOOL CInput_Device::Key_Up(unsigned char byKeyID)
{
	if (m_bKeyState[byKeyID] && !(Get_DIKeyState(byKeyID) & 0x80))
	{
		m_bKeyState[byKeyID] = !m_bKeyState[byKeyID];
		return true;
	}
	if (!m_bKeyState[byKeyID] && (Get_DIKeyState(byKeyID) & 0x80))
		m_bKeyState[byKeyID] = !m_bKeyState[byKeyID];

	return false;
}

BOOL CInput_Device::Mouse_Up(MOUSEKEYSTATE MKS)
{
	if (!m_bMouseState[MKS] && !(Get_DIMouseState(MKS) & 0x80))
	{
		m_bMouseState[MKS] = !m_bMouseState[MKS];
		return true;
	}
	if (!m_bMouseState[MKS] && (Get_DIMouseState(MKS) & 0x80))
		m_bMouseState[MKS] = !m_bMouseState[MKS];

	return false;
}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Update_InputDev(void)
{
	/* �� �����Ӹ��� Ű����� ���콺�� ���¸� �������ش�. */
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

//CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
//{
//	CInput_Device*		pInstance = new CInput_Device();
//
//	if (FAILED(pInstance->Initialize(hInst, hWnd)))
//	{
//		MSG_BOX("Failed to Created : CInput_Device");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

