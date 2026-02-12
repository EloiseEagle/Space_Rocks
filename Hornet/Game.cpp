#include "Game.h"
#include "HtCamera.h"
#include "ObjectManager.h"
#include "HtInput.h"
#include "Rock.h"
#include "Spaceship.h"

void Game::StartOfProgram()
{
    // This makes the operating system's mouse pointer invisible
    // Usually best to use your own.
    HtMouse::instance.SetPointerVisiblity(false);

    if (HtInput::instance.LoadInputFile() == false)
    {
        SetDefaultControls();
    }

    m_gameOver = true;

}

void Game::StartOfGame()
{
    for (int i = 0; i < 5; i++)
    {
        Rock* pRock = new Rock;
        pRock->Initialise();
        ObjectManager::instance.AddItem(pRock);
    }
    

    Spaceship* pSpaceship = new Spaceship;
    pSpaceship->Initialise();
    ObjectManager::instance.AddItem(pSpaceship);
   
    m_gameOver = false;

}

void Game::Update(double frametime)
{
    // Use this section for direct rendering
    // *******************************************************************************


   
    




    // Section below calls the ObjectManager features
    // *******************************************************************************
    ObjectManager::instance.UpdateAll(frametime);
    ObjectManager::instance.ProcessCollisions();
    ObjectManager::instance.RenderAll();

#ifdef _DEBUG
    ObjectManager::instance.CycleDebugObject();
    ObjectManager::instance.RenderDebugDisplay();
#endif
    ObjectManager::instance.DeleteInactiveItems();

    // Draws all graphics to the screen. Best not to remove this line.
    HtGraphics::instance.Present();
}

void Game::OnSuspend()
{

}

void Game::OnResume()
{

}

void Game::EndOfGame()
{
    ObjectManager::instance.DeleteAllObjects();     // Removes all objects from the ObjectManager.
    m_gameOver = false;     // Should already be set to false at this stage, but this makes sure.
}

void Game::EndOfProgram()
{

}


// This will run if the Input system fails to find a controls.ini file.
// You can edit this, but remember it won't do anything if a controls.ini file already exists
void Game::SetDefaultControls()
{
//    HtInput::instance.RemoveAllCommands();

   CommandIndex index = HtInput::instance.AddCommand("UP");
    HtInput::instance.SetKey(index, SDL_SCANCODE_UP);

    index = HtInput::instance.AddCommand("DOWN");
    HtInput::instance.SetKey(index, SDL_SCANCODE_DOWN);

    index = HtInput::instance.AddCommand("LEFT");
    HtInput::instance.SetKey(index, SDL_SCANCODE_LEFT);

    index = HtInput::instance.AddCommand("RIGHT");
    HtInput::instance.SetKey(index, SDL_SCANCODE_RIGHT);

    index = HtInput::instance.AddCommand("FIRE");
    HtInput::instance.SetKey(index, SDL_SCANCODE_SPACE);
    HtInput::instance.SetMouseButton(index, HtMouseButton::LEFT);

    index = HtInput::instance.AddCommand("BOOST");
    HtInput::instance.SetKey(index, SDL_SCANCODE_LCTRL);
    HtInput::instance.SetMouseButton(index, HtMouseButton::RIGHT);

    index = HtInput::instance.AddCommand("ACTION");
    HtInput::instance.SetKey(index, SDL_SCANCODE_RETURN);
    HtInput::instance.SetMouseButton(index, HtMouseButton::MIDDLE);

    HtInput::instance.SaveInputFile();
}

bool Game::IsGameOver() const
{
    return m_gameOver;
}
