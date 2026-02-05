#include "HornetApp.h"
#include "ErrorLogger.h"
#include <iostream>

void Run()
{
    HornetApp theApp;

    if (theApp.Initialise() == Result::FAILURE)
    {
        theApp.SetState(HtProgramState::QUIT);
    }
    else
    {
        theApp.SetState(HtProgramState::MAINMENU);
    }

    // Pump and game loop
    SDL_Event Event;
    while (theApp.GetState() != HtProgramState::QUIT)
    {
        while (SDL_PollEvent(&Event))
        {
            theApp.HandleEvent(Event);
        }

        theApp.Tick();
    }

    theApp.Shutdown();
}

// Curiously, this function actually gets renamed SDL_main by SDL2.
// Is a solution to making a cross platform initialisation of SDL.
// A bit hacky, but it's the sort of thing you have to do in C.
int main(int argc, char* argv[])

{

    // These two lines are for memory leak checking
    // May require Visual Studio
#ifdef _MSC_VER
    _CrtMemState memstart, memend, memdiff;
    _CrtMemCheckpoint(&memstart);
#endif // _MSC_VER

    // This runs the program
    Run();

    // These lines are for memory leak checking
    // May require Visual Studio
#ifdef _MSC_VER
    _CrtMemCheckpoint(&memend);
    if (_CrtMemDifference(&memdiff, &memstart, &memend))
    {
        _CrtDumpMemoryLeaks();
        ErrorLogger::Write("**Memory leaks detected**");
        std::cout << "*************************\n";
        std::cout << "**Memory leaks detected**\n";
        std::cout << "*************************\n\n";
    }

#endif // _MSC_VER

    return 0;
}

