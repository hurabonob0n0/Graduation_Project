#pragma once
#include "Base.h"

#define KEY_NONE        0x00
#define KEY_DOWN        0x01
#define KEY_UP          0x02
#define KEY_PRESSING    0x04

BEGIN(Engine)

class ENGINE_DLL CRawInput_Device : public CBase
{
	DECLARE_SINGLETON(CRawInput_Device)
public:
    CRawInput_Device();
    ~CRawInput_Device() = default;

public:
    HRESULT Initialize(HWND hWnd);
    void Update_InputDev(LPARAM lParam);

    BYTE Get_KeyState(UINT vkey) const { return m_KeyState[vkey]; }
    SHORT Get_MouseDeltaX() const { return m_MouseDelta[0]; }
    SHORT Get_MouseDeltaY() const { return m_MouseDelta[1]; }
    SHORT Get_MouseWheelDelta() const { return m_MouseDelta[2]; }

    bool Key_Down(UINT vkey);
    bool Key_Up(UINT vkey);
    bool Key_Pressing(UINT vkey);

    bool Mouse_Down(UINT button);
    bool Mouse_Up(UINT button);
    bool Mouse_Pressing(UINT button);

    void ResetPerFrame();

    void UpdateKeyStates();

    SHORT Get_Mouse_XDelta() { return m_MouseDelta[0]; }
    SHORT Get_Mouse_YDelta() { return m_MouseDelta[1]; }

private:
    BYTE m_KeyState[256] = {};              // ���� Ű���� ����
    bool m_bKeyPrevState[256] = {};          // ���� ������ Ű���� ����

    bool m_bMouseCurrState[5] = {};          // ���� ���콺 ��ư ���� (��/��/��/X1/X2)
    bool m_bMousePrevState[5] = {};          // ���� ������ ���콺 ��ư ����

    SHORT m_MouseDelta[3] = {};              // ���콺 �̵��� (X, Y, Wheel)
public:
	void Free() override;
};

END