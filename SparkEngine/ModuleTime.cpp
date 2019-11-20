#include "Application.h"
#include "ModuleTime.h"

#define MAX_TIME_SCALE 2.0f

ModuleTime::ModuleTime(bool active): Module(active)
{
}


ModuleTime::~ModuleTime()
{
}

bool ModuleTime::Init(const nlohmann::json::iterator & it)
{
	game_time_clock.Stop();

	return true;
}

void ModuleTime::PrepareUpdate(float app_dt)
{
	frame_count++;

	real_dt = app_dt;

	if (App->IsPaused())
		dt = 0.0f;
	else
		dt = real_dt * time_scale;
}

float ModuleTime::DeltaTime()
{
	return dt;
}

float ModuleTime::Time()
{
	return game_time_clock.Read() / 1000.0f;
}

float ModuleTime::TimeScale()
{
	return time_scale;
}

float ModuleTime::RealTimeSinceStartup()
{
	return real_time_clock.Read() / 1000.0f;
}

float ModuleTime::RealTimeDeltaTime()
{
	return real_dt;
}

float ModuleTime::GetFrameCount()
{
	return frame_count;
}

void ModuleTime::SetTimeScale(float new_scale)
{
	if (new_scale > 0.0f && new_scale < MAX_TIME_SCALE)
		time_scale = new_scale;
}

void ModuleTime::PauseGameClock()
{
	game_time_clock.Pause();
}

void ModuleTime::StopGameClock()
{
	game_time_clock.Stop();
}

void ModuleTime::StartGameClock()
{
	game_time_clock.Start();
}
