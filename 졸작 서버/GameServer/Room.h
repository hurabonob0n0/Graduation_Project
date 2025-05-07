#pragma once
#include "ObjectManager.h"

class Room 
{
public:
	Room();
	Room(uint64 Max);
	~Room();

public:
	void Initialize();
	void Update();
	void LateUpdate();
	void Release();

public:
	void Accept_Player(PlayerRef Player);
	void Leave_Player(PlayerRef Player);
	void Start();
	void Broadcast(SendBufferRef sendBuffer);


	bool Check_Full();
	

public:

	void MovePlayer(int64 playerID, float x, float y, float z);


private:
	
	USE_LOCK;
	map<uint64, PlayerRef>	_Players;
	
	Position PlayersPos[8];

	
	ObjectManager*			Room_ObjectManager;
	uint64					MaxPlayer;

	uint64 testintx = 4;
	uint64 testintY = 7;
};

extern Room GRoom;