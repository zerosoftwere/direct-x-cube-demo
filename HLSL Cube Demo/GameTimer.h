#pragma once
class GameTimer
{
public:
	GameTimer(void);

	float DelatTime() const;
	float TotalTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	operator float() const;

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool	mStopped;

};

