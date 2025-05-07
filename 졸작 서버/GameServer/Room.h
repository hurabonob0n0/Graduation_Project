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

	void ShowPos();

	bool Check_Full();
	

public:

	void MovePlayer(int64 playerID, float x, float y, float z);


private:
	
	USE_LOCK;
	map<uint64, PlayerRef>	_Players;
	uint64					MaxPlayer = 2;

private:
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;

};

extern Room GRoom;