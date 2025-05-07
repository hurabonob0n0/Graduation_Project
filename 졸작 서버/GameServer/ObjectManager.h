#pragma once
#include "GameObject.h"

class ObjectManager
{
	//�� ���� ������Ʈ �Ŵ��� �ϳ��� ������ �ֱ�
private:
	ObjectManager();
	~ObjectManager();

public:
	
	void Add_Object(ObjectID ID, int64 playerID, GameObject* gameObject);

	GameObject* GetGameObject(ObjectID ID, int64 playerID)
	{
		if (ID < 0 || ID >= OBJ_END)
			return nullptr;

		auto& map = _objectMap[ID];
		auto it = map.find(playerID);
		if (it == map.end())
			return nullptr;

		return it->second;
	}

	int Update();
	void Late_Update();
	void Release();

private:
	
	std::unordered_map<uint64, GameObject*> _objectMap[OBJ_END];


};

