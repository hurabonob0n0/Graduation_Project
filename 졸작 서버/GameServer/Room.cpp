#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"
#include "ServerPacketHandler.h"

Room GRoom;

Room::Room() 
{
	
}

Room::Room(uint64 Max) : MaxPlayer(Max)
{

}

Room::~Room()
{
}

void Room::Initialize()
{
	//TODO : 
	
	//Room_ObjectManager->Add_Object()
}

void Room::Update()
{
	//READ_LOCK;
	//empty().Player->queue.pop
	//
	// 
	// 
	//객체들 상호작용, 객체들 상태 업데이트
	//충돌처리



	READ_LOCK
	PlayersPos[0] = Room_ObjectManager->GetGameObject(OBJ_TANK, 0)->GetPos();
	PlayersPos[1] = Room_ObjectManager->GetGameObject(OBJ_TANK, 1)->GetPos();

	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_PLAYER_MOVED
	(PlayersPos[0].PosX, PlayersPos[0].PosY, PlayersPos[0].PosZ, PlayersPos[1].PosX, PlayersPos[1].PosY, PlayersPos[1].PosZ);
	Broadcast(sendBuffer);

}

void Room::LateUpdate()
{
}

void Room::Release()
{
}

void Room::Accept_Player(PlayerRef Player)
{
	WRITE_LOCK;
	_Players[Player->playerID] = Player;
}

void Room::Leave_Player(PlayerRef Player)
{
	WRITE_LOCK;
	_Players.erase(Player->playerID);
}

void Room::Start()
{


	// TODO: Player 인원 확인( 작업)
	// -> 게임 시작 전 이미 맵, 오브젝트들 생성 -> 미리 생성후 이동으로 가져오기
	// -> Player의 선택 여부에 따라 Tank 선택
	// -> 
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (auto& p : _Players) {

		p.second->OwenerSession->Send(sendBuffer);
	}
}

bool Room::Check_Full()
{
	if (_Players.size() >= MaxPlayer) {
		return true;
	}

	return false;
}

void Room::MovePlayer(int64 playerID,float x, float y, float z)
{
	WRITE_LOCK;
	Room_ObjectManager->GetGameObject(OBJ_TANK, playerID)->SetPos(x,y,z);

}	
