#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"


/*-------------------
	For Container
-------------------*/

#include <list>
#include <vector>
#include <set>
#include <map>

/*--------------------------	
	For Server Connection
--------------------------*/



using ClientSessionRef = shared_ptr<class ClientSession>;
using PlayerRef = shared_ptr<class Player>;
