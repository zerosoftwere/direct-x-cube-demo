#include "GameTimer.h"


#include <Windows.h>

GameTimer::GameTimer()
	: mBaseTime(0), mStopTime(0), mPausedTime(0), mPrevTime(0),
		mCurrTime(0), mStopped(false), mDeltaTime(0.0)
{
	__int64 countPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countPerSec);
	mSecondsPerCount = 1.0 / static_cast<double>(countPerSec);
}


void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &currTime);
	
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped  = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &startTime);
	
	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped  = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*) &currTime);
		
		mStopTime = currTime;
		mStopped  = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime  = mCurrTime;

	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

float GameTimer::DelatTime() const
{
	return static_cast<float>(mDeltaTime);
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return static_cast<float>((mStopTime - mPausedTime - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return static_cast<float>((mCurrTime - mPausedTime - mBaseTime) * mSecondsPerCount);
	}
}

GameTimer::operator float() const
{
	return static_cast<float>(mDeltaTime);
}
