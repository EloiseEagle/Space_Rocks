// GameTimer.cpp

#include "GameTimer.h"

GameTimer::GameTimer()
{
	m_freq = SDL_GetPerformanceFrequency();		// Find the timer frequency

	m_minimumFrameTime=0;
	m_maximumFrameTime = 0.1;
	frameTime = 0;
	m_last = SDL_GetPerformanceCounter();
	totalTime = 0;
}

// Use to set the frameTime. Call this once each frame. 
// The function will also delay until the minumum frame time
// has elapsed since the last call to mark.
// Unless the function "setMinimumFrameTime()" has been called
// the minimum time will be zero, in which case, the mark() function will
// not delay.
// Note that
// the frame time is based on the interval between the last two
// calls to mark(). Until this function has been called twice, it
// will be unreliable.
void GameTimer::Mark()
{
	frameTime =0.0;
	Uint64 now=0;
	while(frameTime <= m_minimumFrameTime)	// This is a loop that causes a delay until minimum frame time has elapsed
	{
		now = SDL_GetPerformanceCounter();
		frameTime = (now - m_last) / double(m_freq);
	}
	totalTime += frameTime;
	m_last = now;
	if (frameTime > m_maximumFrameTime)
		frameTime = m_maximumFrameTime;
}

// Sets the minimum frame time (in seconds). The mark() function will delay until
// the minimum time has elapsed since the last call to mark().
// If not set, the minimum time is zero.
void GameTimer::SetMinimumFrameTime(double minTime)
{
	if(minTime>0.0)      // Can't have a negative minimum time
	{
		m_minimumFrameTime = minTime;
	}
	else 
		m_minimumFrameTime =0.0;
}

void GameTimer::SetMaximumFrameTime(double maxTime)
{
	m_maximumFrameTime = maxTime;
}
