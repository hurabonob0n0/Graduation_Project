#pragma once
#include "GameObject.h"

class ObjectManager
{
	//룸 마다 오브젝트 매니저 하나씩 가지고 있기
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

public:
	static	ObjectManager* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new ObjectManager;
		}
		return m_pInstance;
	}

	static void			Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

	int Update();
	void Late_Update();
	void Release();

private:
	static	ObjectManager* m_pInstance;

	std::unordered_map<uint64, GameObject*> _objectMap[OBJ_END];


};

