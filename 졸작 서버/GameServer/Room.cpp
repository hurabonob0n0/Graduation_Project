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

Room::Room(uint64 Max)
{

}

Room::~Room()
{
}

void Room::Initialize()
{
	WRITE_LOCK;
	ObjectManager::Get_Instance()->Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(0, 0, 0));
	ObjectManager::Get_Instance()->Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(0, 0, 0));

	//SendBufferRef sendBuffer0 = ServerPacketHandler::Make_S_ROOMCREATED(1);
	//_Players[0]->OwenerSession->Send(sendBuffer0);

	//SendBufferRef sendBuffer1 = ServerPacketHandler::Make_S_ROOMCREATED(2);
	//_Players[1]->OwenerSession->Send(sendBuffer1);
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


	// TODO: Player �ο� Ȯ��( �۾�)
	// -> ���� ���� �� �̹� ��, ������Ʈ�� ���� -> �̸� ������ �̵����� ��������
	// -> Player�� ���� ���ο� ���� Tank ����
	// -> 
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (const auto& iter : _Players)
	{
		const PlayerRef& player = iter.second;
		if (player && player->OwenerSession)
			player->OwenerSession->Send(sendBuffer);
	}
}

void Room::ShowPos()
{
	READ_LOCK;
	Position OBJ0 = (*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[0]->GetPos();
	Position OBJ1 = (*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[1]->GetPos();

	cout << "OBJ0" << "   " << OBJ0.PosX << "     " << OBJ0.PosY << "    " << OBJ0.PosZ << endl;
	cout << "OBJ1" << "   " << OBJ1.PosX << "     " << OBJ1.PosY << "    " << OBJ1.PosZ << endl;


	//Vec3_Data Pos[2];
	//Pos[0] = { OBJ0 .PosX, OBJ0.PosY,OBJ0.PosZ};
	//Pos[1] = { OBJ1.PosX, OBJ1.PosY,OBJ1.PosZ };
	//SendBufferRef sendBuffer = ServerPacketHandler::Make_S_PLAYER_MOVED(Pos);

	//Broadcast(sendBuffer);

	//SendBufferRef sendBuffer1 = ServerPacketHandler::Make_S_PLAYER_MOVED(OBJ1.PosX,OBJ1.PosY,OBJ1.PosZ);
	//_Players[0]->OwenerSession->Send(sendBuffer1);

	//SendBufferRef sendBuffer2 = ServerPacketHandler::Make_S_PLAYER_MOVED(OBJ0.PosX, OBJ0.PosY, OBJ0.PosZ);
	//_Players[1]->OwenerSession->Send(sendBuffer2);
}

bool Room::Check_Full(uint16 MaxPlayer)
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
	(*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[playerID]->SetPos(x,y,z);

}

void Room::SetPlayerPos(int64 pID, float x, float y, float z)
{
	WRITE_LOCK;
	(*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[pID]->SetPos(x, y, z);

}

Vec3_Data Room::GetPlayerPos(int64 pID)
{
	READ_LOCK;
	Vec3_Data Temp;
	Temp.X = (*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[pID]->GetPos().PosX;
	Temp.Y = (*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[pID]->GetPos().PosY;
	Temp.Z = (*ObjectManager::Get_Instance()->Get_List(OBJ_TANK))[pID]->GetPos().PosZ;

	return Temp;
}

