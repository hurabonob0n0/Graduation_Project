#pragma once

enum
{
	S_TEST = 1,
	S_SUCCES_LOGIN = 2,
	S_PLAYER_MOVE = 3,
	C_LOGIN = 1001,
	C_KEYINPUT = 1002,
	C_MOVEMENT = 1003
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	static void Handle_S_TEST(BYTE* buffer, int32 len);
	static void Handle_S_SUCCES_LOGIN(BYTE* buffer, int32 len);
	static void Handle_S_PLAYER_MOVE(BYTE* buffer, int32 len);

	static SendBufferRef Make_C_LOGIN(uint64 id, uint32 hp, uint16 attack);
	static SendBufferRef Make_C_KEYINPUT(uint8 key);
	static SendBufferRef Make_C_MOVE(float x, float y, float z);

};
