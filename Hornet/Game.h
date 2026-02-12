#pragma once
#include <vector>
#include "HtGraphics.h"
#include "HtKeyboard.h"
#include "HtMouse.h"
#include "HtAudio.h"
#include "GameTimer.h"
#include "HtInput.h"

// 17/06/2023
// This class is where you will start programming a game.
// The core of the game loop is Update(), which is called from HornetApp.
//
// 6/12/2023
// Added engine #includes to the header files. We are going to need them anyway.
// 
// 3/6/2025    Added SetMaximumFPS.

// 09/01/2026
// Added game over functionality



class Game
{
public:
    // Use this function for any code when the program starts.
    // This could be empty
    void StartOfProgram();

    // Use this function to set up your game each time it starts
    void StartOfGame();

    // This function runs one "frame of animation".
    // Actually, in a windows environment it will probably
    // run much more often than that.
    // double frametime - the time in seconds since the last frame
    void Update(double frameTime);

    // This function is for when the program goes to the pause menu
    // You may want to pause sound and music effects, or set the
    // background colour
    void OnSuspend();

    // This function is for when the program returns from the pause menu
    // You may want to resume sound and music effects, or set the
    // background colour
    void OnResume();

    // Use this function to delete any objects at the end of the game.
    // You may want to stop sound and music.
    void EndOfGame();

    // Use this function to delete any objects before the program exits.
    // This may well be empty
    void EndOfProgram();

    // This will run if the Input system fails to find a controls.ini file.
    // You can edit this, but remember it won't do anything if a controls.ini file exists
    void SetDefaultControls();

    // Return true when game should end
    bool IsGameOver() const;   

private:
    bool m_gameOver;        // If set to true, the game will run EndOfGame( ) and return to the main menu.
};

