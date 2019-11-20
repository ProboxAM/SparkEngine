#ifndef _MODULE_TIME_H_
#define _MODULE_TIME_H_

#include "Timer.h"
#include "Module.h"

class ModuleTime :
	public Module
{
public:
	ModuleTime(bool );
	~ModuleTime();

	bool Init(const nlohmann::json::iterator& it);

	void PrepareUpdate(float dt);

	float DeltaTime();
	float Time();
	float TimeScale();

	float RealTimeSinceStartup();
	float RealTimeDeltaTime();

	float GetFrameCount();
	void SetTimeScale(float new_scale);
	void PauseGameClock();
	void StopGameClock();
	void StartGameClock();

private:
	Timer real_time_clock;
	Timer game_time_clock;

	float real_dt = 0.0f;
	float dt = 0.0f;

	uint frame_count = 0;
	float time_scale = 1;
};


#endif // !_MODULE_TIME_H_


