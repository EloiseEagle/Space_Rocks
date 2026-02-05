// GameTimer.h
// Hornet Engine
// 1/5/2023 First created by adapting from the Shell Engine timer


#pragma once
#include "SDL.h"

// This class will allow you to measure the frame time.
// Can also be used to measure any other short intervals.
class GameTimer
{

public:
	double frameTime;		// The duration between the last two uses of the mark() function in seconds
	double totalTime;		// Total time in seconds since the object was created

	// Constructor
	GameTimer();

		// Sets the minimum frame time (in seconds). The mark() function will delay until
		// the minimum time has elapsed since the last call to mark().
		// In effect, this will slow the game down if it runs too fast.
		// If not set, the minimum time is zero.
	void SetMinimumFrameTime(double minTime);


	// Sets the maximum frame time (in seconds). This won't force the program to run faster
	// if the program is running slowly. What it will do is limit the value of frametime.
	// If your program has a long gap between one frame and the next (for example if the program
	// has been suspended for a while), this will prevent your game physics from being affected
	// by a very long frame time. 
	// By default the maximum frametime is 0.1.
	void SetMaximumFrameTime(double maxTime);
							
// Use to set the frameTime. Call this once each frame. 
// The function will also delay until the minumum frame time
// has elapsed since the last call to mark.
// Unless the function "setMinimumFrameTime()" has been called
// the minimum time will be zero, in which case, the mark() function will
// not delay.
// The duration of the frame will be in the public member variable mdFrameTime.
// This will be limited to a maxumum value of 0.1, but this limit can be changed
// by using SetMaximumFrameTime.
// Note that
// the frame time is based on the interval between the last two
// calls to mark(). Until this function has been called twice, it
// will be unreliable.
	void Mark();


private:
	Uint64 m_freq;		// Stores the frequency of the performance counter ("ticks" per second)
	Uint64 m_last;		// Stores the time of the last mark time (in ticks)
	double m_minimumFrameTime;	// The minumim frame time that mark() will allow
	double m_maximumFrameTime;	// The maximum frame time that mark() will allow					
};
