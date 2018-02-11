#include "Timer.h"
#include "SDL.h"

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;

    paused = false;
    started = false;
}

void Timer::start()
{
	paused = false;
    started = true;
    pausedTicks = 0;
	startTicks = SDL_GetTicks();
}

void Timer::stop()
{
	paused = false;
    started = false;
    startTicks = 0;
    pausedTicks = 0;
}

void Timer::pause()
{
    if(!paused && started)
    {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void Timer::unpause()
{
    if( started && paused )
    {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

unsigned int Timer::getTicks()
{
	unsigned int time = 0;

    if( started )
    {
        if( paused )
        {
            time = pausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - startTicks;
        }
    }

    return time;
}

bool Timer::isStarted()
{
    return started;
}

bool Timer::isPaused()
{
    return paused && started;
}