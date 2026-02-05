#pragma once
#include <string>
#include "Result.h"
#include "Game.h"
#include "HornetMenus.h"

//6/12/2023
// Added calls to start and end the program in Game

// 09/01/2026
// Added the game over functionality to tick()

// Tracks the current state of the program - initialising, in the main menus
// (including credits and options), in the pause menu, playing the game or quitting
enum class HtProgramState{INITIALISE, MAINMENU, PAUSEMENU, GAMEPLAY, QUIT};

const std::string APPNAME = "Space Rocks";
const std::string ASSETS = "assets\\";
const std::string DOCS = "docs\\";
const std::string FONTS = "fonts\\";

// This class runs a game loop and the message pump.
// You probably won't need to change this class, but may want to
// change the APPNAME above.
class HornetApp
{
public:
    // Initialises SDL and the Ht engines.
    // Returns FAILURE if SDL or any of the Ht engines fail to initialise.
    Result Initialise();

    // Handles SDL messages. Called within Tick( ) as the message pump
    // SDL_QUIT - puts the program into the QUIT state
    // Keyboard events - passes them to HtKeyboard
    // Mouse events - passes them to HtMouse
    void HandleEvent(SDL_Event& evt);

    // Changes the program to the specified state
    void SetState(HtProgramState newState);

    // Returns the current state of the program
    HtProgramState GetState() const;

    // This is the main loop of the program. Measures the frame time
    // and updates either the HornetMenu or the Game.
    void Tick();

    // Shuts down the menu. Shuts down HtGraphics, HtAudio and HtKeyboard.
    // Other engines don't need any shutdown code.
    void Shutdown();
private:
    HtProgramState m_programState = HtProgramState::INITIALISE;
    Game m_theGame;
    HornetMenus m_theMenus;
};



