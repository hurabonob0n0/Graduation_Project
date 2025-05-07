#include "pch.h"
#include "ObjectManager.h"

void ObjectManager::Add_Object(ObjectID ObjID, int64 playerID, GameObject* gameObject)
{
	if (OBJ_END <= ObjID || gameObject == nullptr)
		return;

	_objectMap[ObjID][playerID] = gameObject;
}

int ObjectManager::Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = _objectMap[i].begin(); iter != _objectMap[i].end(); ++iter)
		{
			GameObject* obj = iter->second;
			if (obj)
				obj->Update();
		}
	}

	return true;
}

void ObjectManager::Late_Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = _objectMap[i].begin(); iter != _objectMap[i].end(); ++iter)
		{
			GameObject* obj = iter->second;
			if (obj)
				obj->Late_Update();
		}
	}
}

void ObjectManager::Release()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = _objectMap[i].begin(); iter != _objectMap[i].end(); ++iter)
		{
			GameObject* obj = iter->second;
			if (obj)
				Safe_Delete(obj);
		}
		_objectMap[i].clear();
	}
}