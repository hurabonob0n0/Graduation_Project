#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
	DECLARE_SINGLETON(CTimer)

public:
	CTimer();

	float TotalTime() const; // in seconds
	float DeltaTime() const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;

	// CBase을(를) 통해 상속됨
	void Free() override;
};

END