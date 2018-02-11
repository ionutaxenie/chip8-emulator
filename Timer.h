class Timer
{
    public:
        
        Timer();

        void start();
        void stop();
        void pause();
        void unpause();

        unsigned int getTicks();

        bool isStarted();
        bool isPaused();

    private:
		unsigned int startTicks;
		unsigned int pausedTicks;

        bool paused;
        bool started;
};