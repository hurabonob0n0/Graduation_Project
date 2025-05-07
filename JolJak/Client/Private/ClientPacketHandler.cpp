#include "pch.h"
#include "CGlobals.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "ServiceManager.h"
#include "GameInstance.h"



ClientPacketHandler::ClientPacketHandler()
{
}

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	case S_SUCCES_LOGIN:
		Handle_S_SUCCES_LOGIN(buffer, len);
		break;
	case S_PLAYER_MOVE:
		Handle_S_PLAYER_MOVE(buffer, len);
		break;
	case S_GAME_START:
		Handle_S_GAMESTART(buffer, len);
		break;

	}
}


struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;

};

struct S_Pos {
	float PosX;
	float PosY;
	float PosZ;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	br >> id >> hp >> attack;

	cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;
}

void ClientPacketHandler::Handle_S_SUCCES_LOGIN(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);
	PacketHeader header;

	br >> header;
	uint16 ID;

#pragma region ID�� ��ü�� �־��ֱ�

	br >> ID;
	ServiceManager::GetInstace().SetMyID(ID);
	g_ServerConnected.store(true);
	g_PlayerID.store(ID);

#pragma endregion
}

void ClientPacketHandler::Handle_S_PLAYER_MOVE(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	S_Pos PlayersPos;
	br >> PlayersPos.PosX >> PlayersPos.PosY >> PlayersPos.PosZ;


	px.store(PlayersPos.PosX);
	py.store(PlayersPos.PosY);
	pz.store(PlayersPos.PosZ);
	


#pragma region ��ü�� ��ǥ�� �־��ֱ�
	//uint16 id = ServiceManager::GetInstace().GetMyID();



#pragma endregion TODO : LOCK�ɰ� PlayerID�� �´� �� �־��ֱ�
}

void ClientPacketHandler::Handle_S_GAMESTART(BYTE* buffer, int32 len)
{
	
	int a = 0;

}

SendBufferRef ClientPacketHandler::Make_C_LOGIN(uint64 id, uint32 hp, uint16 attack)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << id << hp << attack;

	header->size = bw.WriteSize();
	header->id = C_LOGIN;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ClientPacketHandler::Make_C_KEYINPUT(uint8 key)
{

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << key;

	header->size = bw.WriteSize();
	header->id = C_KEYINPUT;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ClientPacketHandler::Make_C_MOVE(float x, float y, float z)
{
	
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());
	PacketHeader* header = bw.Reserve<PacketHeader>();
	
	bw << x << y << z;

	header->size = bw.WriteSize();
	header->id = C_MOVEMENT;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
	
}
