#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"
#include "ServerPacketHandler.h"
#include "AbstractFactory.h"
#include "Tank.h"
#include "ObjectManager.h"
#include "SGlobal.h"

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

	ObjectManager::Get_Instance()->Add_Object(OBJ_TANK, 0, CAbstractFactory<Tank>::Create(0,0,0));
	ObjectManager::Get_Instance()->Add_Object(OBJ_TANK, 1, CAbstractFactory<Tank>::Create(0, 0, 0));

	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_GAMESTART(1);
	Broadcast(sendBuffer);

}

void Room::Update()
{

	//READ_LOCK
	//Position Pos0 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 0)->GetPos();
	//Position Pos1 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 1)->GetPos();


	//x1 = Pos0.PosX;
	//y1 = Pos0.PosY;
	//z1 = Pos0.PosZ;
	//x2 = Pos1.PosX;
	//y2 = Pos1.PosY;
	//z2 = Pos1.PosZ;
}

void Room::LateUpdate()
{

	//READ_LOCK;
	//Position OBJ0 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 0)->GetPos();
	//Position OBJ1 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 1)->GetPos();
	//SendBufferRef sendBuffer = ServerPacketHandler::Make_S_PLAYER_MOVED(OBJ0.PosX, OBJ0.PosY, OBJ0.PosZ, OBJ1.PosX, OBJ1.PosY, OBJ1.PosZ);
	//Broadcast(sendBuffer);

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

void Room::ShowPos()
{
	READ_LOCK;
	Position OBJ0 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 0)->GetPos();
	Position OBJ1 = ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, 1)->GetPos();

	cout << "OBJ0" << "   " << OBJ0.PosX << "     " << OBJ0.PosY << "    " << OBJ0.PosZ << endl;
	cout << "OBJ1" << "   " << OBJ1.PosX << "     " << OBJ1.PosY << "    " << OBJ1.PosZ << endl;



	SendBufferRef sendBuffer1 = ServerPacketHandler::Make_S_PLAYER_MOVED(OBJ1.PosX,OBJ1.PosY,OBJ1.PosZ);
	_Players[0]->OwenerSession->Send(sendBuffer1);

	SendBufferRef sendBuffer2 = ServerPacketHandler::Make_S_PLAYER_MOVED(OBJ0.PosX, OBJ0.PosY, OBJ0.PosZ);
	_Players[1]->OwenerSession->Send(sendBuffer2);
}

bool Room::Check_Full()
{
	READ_LOCK;
	if (_Players.size() >= MaxPlayer) {
		g_Roomfull.store(true);
		return true;
	}

	return false;
}

void Room::MovePlayer(int64 playerID,float x, float y, float z)
{
	WRITE_LOCK;
	ObjectManager::Get_Instance()->GetGameObject(OBJ_TANK, playerID)->SetPos(x,y,z);

}	
