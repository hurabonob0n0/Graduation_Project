#pragma once

enum
{
	S_TEST = 1,//for Dummy
	S_SUCCES_LOGIN = 2,
	S_PLAYER_MOVE = 3,
	C_LOGIN = 1001,
	C_KEYINPUT = 1002, 
	C_MOVEMENT = 1003
};

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

	//For Recv
	static void Handle_C_LOGIN(PacketSessionRef& session,BYTE* buffer, int32 len);
	static void Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len);
	

	//For Send
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack);
	static SendBufferRef Make_S_SUCCES_LOGIN(uint16 id);
	static SendBufferRef Make_S_PLAYER_MOVED(float x1, float y1, float z1, float x2, float y2, float z2);
};

