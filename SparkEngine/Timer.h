#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Stop();

	void Pause();
	Uint32 Read();

private:

	bool	paused = false;
	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};

#endif //__TIMER_H__