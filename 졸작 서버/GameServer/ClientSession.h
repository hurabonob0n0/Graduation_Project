#pragma once
#include "Session.h"

class ClientSession : public PacketSession
{
public:
	~ClientSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	Vector<PlayerRef> _players;
	//�� ������ ���� �÷��̾ ������ ���� �� ����
	//TODO: DB ���� �� �ʿ�
};