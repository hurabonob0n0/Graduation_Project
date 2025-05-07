#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include "Room.h"
#include <chrono>
#include <thread>

using namespace std::chrono;

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100);

	ASSERT_CRASH(service->Start());
	int32 threadCount = std::thread::hardware_concurrency();

	GRoom.Initialize();

	for (int32 i = 0; i < threadCount; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}				
			});
	}	

	constexpr int64_t FRAME_DURATION_MS = 33;

	while (true)
	{
		auto frameStart = high_resolution_clock::now();

		GRoom.Update();
		GRoom.LateUpdate();
		//GRoom.Broadcast();

		auto frameEnd = high_resolution_clock::now();
		auto elapsed = duration_cast<milliseconds>(frameEnd - frameStart).count();

		if (elapsed < FRAME_DURATION_MS)
		{
			std::this_thread::sleep_for(milliseconds(FRAME_DURATION_MS - elapsed));
		}
	}
	
	//while (true)
	//{
	//	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 100, 10);
	//	GSessionManager.Broadcast(sendBuffer);
	//	this_thread::sleep_for(250ms);
	//}

	GThreadManager->Join();
}