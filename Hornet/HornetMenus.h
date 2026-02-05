#pragma once
#include "HtGraphics.h"
#include "HtAudio.h"
#include <vector>

class HornetApp;

// This is a fairly crude class to manage a basic menu system.
// Currently includes the main menu, the credits menu and the pause menu.
// I will probably add settings menus later, but this class is almost a 
// placeholder.

// Modified 19/04/24
// Stopped menus from using the camera position

// Modified 09/01/26
// Added the menu for adjusting the inputs

class HornetMenus
{
public:
    void Initialise();
    void Update(double frameTime, HornetApp* pOwner);
    void Shutdown();
    void SwitchToPause();
    void SwitchToMain();
    void SwitchToCredits();
    void SwitchToKeys();
    void DisplayLoadScreen();
    HornetMenus();
    ~HornetMenus();


private:
    static const std::string KEYFILE;
    enum class MenuType{MAIN, PAUSE, CREDITS, KEYS};
    void UpdateMain(double frametime, HornetApp* pOwner);
    void UpdatePause(double frametime, HornetApp* pOwner);
    void UpdateCredits(double frametime, HornetApp* pOwner);
    void UpdateKeys(double frametime, HornetApp* pOwner);

    int m_currentOption;
    double m_creditFlowTimer;
    MenuType m_currentType;
    std::vector<std::string> m_mainOptionText;
    std::vector<std::string> m_pauseOptionText;
    std::vector<std::string> m_creditsText;

    SoundIndex m_clickSound;
    SoundIndex m_selectSound;

    int m_selectedInputRow;         // Which row in the input table is currently selected
    int m_selectedInputColumn;      // Which row in the input table is currently selected (0=keys, 1=mouse, 2=joypad)
    int m_inputTableTop;            // Normally zero, unless scrolled up and down
    bool m_waitingForInputChoice;   // If true, waiting for user to enter a key, mouse button or joypad button
};

