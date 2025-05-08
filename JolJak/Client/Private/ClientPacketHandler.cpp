#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "ServiceManager.h"

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

#pragma region ID값 객체에 넣어주기

	br >> ID;
	ServiceManager::GetInstace().SetMyID(ID);

#pragma endregion
}

void ClientPacketHandler::Handle_S_PLAYER_MOVE(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	S_Pos PlayersPos[2];
	br >> PlayersPos[0].PosX >> PlayersPos[0].PosY >> PlayersPos[0].PosZ 
		>> PlayersPos[1].PosX >> PlayersPos[1].PosY >> PlayersPos[1].PosZ;

#pragma region 객체들 좌표값 넣어주기
	uint16 id = ServiceManager::GetInstace().GetMyID();
	//ex) objectlist.Get_object(objbox,id).set_pos(PlayersPos[id]);

#pragma endregion TODO : LOCK걸고 PlayerID에 맞는 값 넣어주기
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
