#include "HornetMenus.h"
#include "HtKeyboard.h"
#include "HtInput.h"
#include "HornetApp.h"
#include "ErrorLogger.h"
#include "HtCamera.h"
#include <fstream>


const std::string HornetMenus::KEYFILE = "keys.txt";

void HornetMenus::Initialise()
{

    m_currentOption = 0;
    m_currentType = MenuType::MAIN;

    m_mainOptionText.push_back("Start game");
    m_mainOptionText.push_back("Controls");
    m_mainOptionText.push_back("Credits");
    m_mainOptionText.push_back("Exit");

    m_pauseOptionText.push_back("Resume");
    m_pauseOptionText.push_back("Exit");

    std::ifstream file;
    file.open(DOCS+"credits.txt");
    if (!file.is_open())
    {
        ErrorLogger::Write("credits.txt not found in "+DOCS+" folder.");
    }
    else
    {
        std::string line;
        while (std::getline(file, line)) 
        {
            m_creditsText.push_back(line);
        }
    }
    file.close();

    m_clickSound= HtAudio::instance.LoadSound(ASSETS + "buttonClick.wav");
    m_selectSound = HtAudio::instance.LoadSound(ASSETS + "buttonChoice.wav");;
}

void HornetMenus::Update(double frameTime, HornetApp* pOwner)
{
    if (m_currentType == MenuType::MAIN)
    {
        UpdateMain(frameTime, pOwner);
    }
    else if (m_currentType == MenuType::PAUSE)
    {
        UpdatePause(frameTime, pOwner);
    }
    else if (m_currentType == MenuType::CREDITS)
    {
        UpdateCredits(frameTime, pOwner);
    }
    else if (m_currentType == MenuType::KEYS)
    {
        UpdateKeys(frameTime, pOwner);
    }

    HtGraphics::instance.Present();
}

void HornetMenus::Shutdown()
{
    m_creditsText.clear();
    m_pauseOptionText.clear();
    m_mainOptionText.clear();
}

void HornetMenus::SwitchToPause()
{
    m_currentType = MenuType::PAUSE;
}

void HornetMenus::SwitchToMain()
{
    m_currentType = MenuType::MAIN;
}

void HornetMenus::SwitchToCredits()
{
    m_currentType = MenuType::CREDITS;
    m_creditFlowTimer = 0;
}

void HornetMenus::SwitchToKeys()
{
    m_currentType = MenuType::KEYS;

    m_selectedInputRow = 0;
    m_selectedInputColumn = 0;
    m_inputTableTop = 0;
    m_waitingForInputChoice = false;
}

void HornetMenus::DisplayLoadScreen()
{
    HtGraphics::instance.Present();
    HtGraphics::instance.WriteTextAligned(Vector2D(-200, 50), "LOADING",
        HtGraphics::RED, 4, 3.0);

    HtGraphics::instance.Present();
}

HornetMenus::HornetMenus()
{
    m_clickSound = HtAudio::NOSOUND;
    m_selectSound = HtAudio::NOSOUND;
    m_creditFlowTimer = 0;
    m_currentOption=0;
    m_currentType= MenuType::MAIN;
}

HornetMenus::~HornetMenus()
{
}

void HornetMenus::UpdateMain(double frameTime, HornetApp* pOwner)
{
    bool cameraActive = HtCamera::instance.IsCameraActive();
    HtCamera::instance.UseCamera(false);

    HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 750), APPNAME, HtGraphics::RED, 0, 2.0);

    for (int i = 0; i < m_mainOptionText.size(); ++i)
    {
        if (i == m_currentOption)
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000.0, double(625.0-120.0*i)), m_mainOptionText[i],
                HtGraphics::BLUE, 0, 2.4f);
        }
        else
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000.0, double(600.0-120.0 * i)), m_mainOptionText[i],
                HtGraphics::LIGHTBLUE, 0, 2.0);
        }
    }

    // Handle player choice
    static bool input = true;
    if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_UP))
    {
        if (input == false)
        {
            HtAudio::instance.Play(m_clickSound);
            m_currentOption--;
            if (m_currentOption < 0)
                m_currentOption = (int)m_mainOptionText.size()-1;
        }
        input = true;
    }
    else if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_DOWN))
    {
        if (input == false)
        {
            HtAudio::instance.Play(m_clickSound);
            ++m_currentOption;
            if (m_currentOption>= m_mainOptionText.size())
                m_currentOption = 0;
        }
        input = true;
    }
    else if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_RETURN))
    {
        if (input == false)
        {
            HtAudio::instance.Play(m_selectSound);
            // Handle option choice
            if (m_currentOption == 0)
                pOwner->SetState(HtProgramState::GAMEPLAY);
            else if (m_currentOption == 1)
                SwitchToKeys();
            else if (m_currentOption == 2)
                SwitchToCredits();
            else if (m_currentOption == 3)
                pOwner->SetState(HtProgramState::QUIT);
        }
        input = true;
    }
    else
    {
        input = false;
    }
    HtCamera::instance.UseCamera(cameraActive);
}

void HornetMenus::UpdatePause(double frametime, HornetApp* pOwner)
{
    bool cameraActive = HtCamera::instance.IsCameraActive();
    HtCamera::instance.UseCamera(false);
    HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 750), "PAUSED", HtGraphics::RED, 0, 2.0);

    for (int i = 0; i < m_pauseOptionText.size(); ++i)
    {
        if (i == m_currentOption)
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000.0, 625.0 - double(120.0 * i)), m_pauseOptionText[i],
                HtGraphics::BLUE, 0, 2.4f);
        }
        else
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000.0, 600.0 - double(120.0 * i)), m_pauseOptionText[i],
                HtGraphics::LIGHTBLUE,0, 2.0);
        }
    }

    // Handle player choice
    static bool input = true;
    if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_UP))
    {
        if (input == false)
        {
            HtAudio::instance.Play(m_clickSound);
            m_currentOption--;
            if (m_currentOption < 0)
                m_currentOption = (int)m_pauseOptionText.size()-1;
        }
        input = true;
    }
    else if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_DOWN))
    {
        if (input == false)
        {
            HtAudio::instance.Play(m_clickSound);
            ++m_currentOption;
            if (m_currentOption >= m_pauseOptionText.size())
                m_currentOption = 0;
        }
        input = true;
    }
    else if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_RETURN))
    {
        if (input == false)
        {
            // Handle option choice
            if (m_currentOption == 0)
            {
                pOwner->SetState(HtProgramState::GAMEPLAY);
                HtAudio::instance.Play(m_selectSound);
            }

            else if (m_currentOption == 1)
            {
                pOwner->SetState(HtProgramState::MAINMENU);
                HtAudio::instance.Play(m_selectSound);
            }
        }
        input = true;
    }
    else
    {
        input = false;
    }
    HtCamera::instance.UseCamera(cameraActive);
}

void HornetMenus::UpdateCredits(double frametime, HornetApp* pOwner)
{
    bool cameraActive = HtCamera::instance.IsCameraActive();
    HtCamera::instance.UseCamera(false);
    const int MAXLINES = 20;

    m_creditFlowTimer += frametime;
    HtGraphics::instance.WriteTextAligned(Vector2D(-1200.0, 800.0), "CREDITS", HtGraphics::RED, 0, 2.0);
    HtGraphics::instance.WriteTextAligned(Vector2D(600.0, -900.0), "Enter to return", HtGraphics::RED, 0, 2.0);

    int currentRow = int(m_creditFlowTimer);
    int part = int((m_creditFlowTimer - currentRow) * 50.0);

    if (currentRow - MAXLINES > int(m_creditsText.size()))
    {
        m_creditFlowTimer = 0;
    }
    for (int i = 0; i < m_creditsText.size(); ++i)
    {

        if (i < currentRow && i>currentRow-MAXLINES)
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1200.0, -600- 50 * (double(i) - m_creditFlowTimer)), m_creditsText[i],
                HtGraphics::WHITE, 2, 1.0);
        }
        else if (i == currentRow)
        {
            std::string text = m_creditsText[i];// .substr(0, part * 2);
            for (char& c : text)
            {
                if (c<80-part || c>80+part)
                    c = ' ';
            }
            Colour c(5 * part, 255, 255, 255);
            HtGraphics::instance.WriteTextAligned(Vector2D(-1200.0, -600-50 * (double(i) - m_creditFlowTimer)), text,
                c, 2, 1.0);
        }
        else if (i == currentRow - MAXLINES)
        {
            int oppart = 50 - part;
            std::string text = m_creditsText[i];
            for (char& c : text)
            {
                if (c < 80 - oppart || c>80 + oppart)
                    c = '*';
            }
            Colour c(255 - 5 * part, 255, 255, 255);
            HtGraphics::instance.WriteTextAligned(Vector2D(-1200.0, -600 - 50 * (double(i) - m_creditFlowTimer)), text,
                c, 2, 1.0);
        }
    }
    // Handle player choice
    static bool input = true;
    if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_RETURN) ||
        HtKeyboard::instance.KeyPressed(SDL_SCANCODE_ESCAPE))
    {
        if (input == false)
        {
            // Handle option choice
            SwitchToMain();
            HtAudio::instance.Play(m_clickSound);
        }
        input = true;
    }
    else
    {
        input = false;
    }
    HtCamera::instance.UseCamera(cameraActive);
}

void HornetMenus::UpdateKeys(double frametime, HornetApp* pOwner)
{
    int numCommands = HtInput::instance.GetNumCommands();
    if (!m_waitingForInputChoice&&HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_ESCAPE))
    {

            HtInput::instance.SaveInputFile();
            SwitchToMain();
    }

    HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 750), "Control settings", HtGraphics::RED, 0, 2.0);
    if (m_waitingForInputChoice)
    {
        if(m_selectedInputColumn==0)
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 650), "Press a key", HtGraphics::YELLOW, 0, 1.0);
        if (m_selectedInputColumn == 1)
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 650), "Press a mouse button", HtGraphics::YELLOW, 0, 1.0);
        if (m_selectedInputColumn == 2)
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 650), "Press a controller button", HtGraphics::YELLOW, 0, 1.0);
        HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 600), "Or Escape to exit", HtGraphics::YELLOW, 0, 1.0);
    }
    else
    {
        HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 650), "Use cursor keys and enter to select", HtGraphics::WHITE, 0, 1.0);
        HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 600), "Or Escape to exit", HtGraphics::WHITE, 0, 1.0);
    }

    // Display table headers
    HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 500), "Command", HtGraphics::WHITE, 0, 1.5);
    HtGraphics::instance.WriteTextAligned(Vector2D(-300, 500), "Keyboard", HtGraphics::LIGHTBLUE, 0, 1.5);
    HtGraphics::instance.WriteTextAligned(Vector2D(100, 500), "Mouse", HtGraphics::LIGHTGREEN, 0, 1.5);
    HtGraphics::instance.WriteTextAligned(Vector2D(500, 500), "Controller", HtGraphics::LIGHTRED, 0, 1.5);

    // Draw the selector box
    Rectangle2D selector;
    selector.PlaceAt(340 - (m_selectedInputRow- m_inputTableTop) * 75, 
        -300 + m_selectedInputColumn * 400,
        265 - (m_selectedInputRow - m_inputTableTop) * 75,
        100 + m_selectedInputColumn * 400);

    // If the box is "active" and waiting for a new key input
    if (m_waitingForInputChoice && numCommands>0)
    {
        HtGraphics::instance.FillRect(selector, HtGraphics::LIGHTRED);

        if (m_selectedInputColumn == 0)    // Keyboard
        {
            if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                HtInput::instance.SetKey(m_selectedInputRow, SDL_SCANCODE_UNKNOWN);
                m_waitingForInputChoice = false;
            }
            else
            {
                // Check all possible keys
                for (int i = 0; i < 255; i++)
                {
                    if (HtKeyboard::instance.NewKeyPressed((SDL_Scancode)i))
                    {
                        HtInput::instance.SetKey(m_selectedInputRow, (SDL_Scancode)i);
                        m_waitingForInputChoice = false;
                    }
                }
            }
        }
        else if (m_selectedInputColumn == 1)    // Mouse
        {
            if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                HtInput::instance.SetMouseButton(m_selectedInputRow, HtMouseButton::NONE);
                m_waitingForInputChoice = false;
            }
            else
            {
                // Check three mouse buttons

                if (HtMouse::instance.IsNewMouseDown(HtMouseButton::LEFT))
                {
                    HtInput::instance.SetMouseButton(m_selectedInputRow, HtMouseButton::LEFT);
                    m_waitingForInputChoice = false;
                }

                if (HtMouse::instance.IsNewMouseDown(HtMouseButton::RIGHT))
                {
                    HtInput::instance.SetMouseButton(m_selectedInputRow, HtMouseButton::RIGHT);
                    m_waitingForInputChoice = false;
                }
                if (HtMouse::instance.IsNewMouseDown(HtMouseButton::MIDDLE))
                {
                    HtInput::instance.SetMouseButton(m_selectedInputRow, HtMouseButton::MIDDLE);
                    m_waitingForInputChoice = false;
                }
            }
        }
        else if (m_selectedInputColumn == 2)    // Controller
        {
            if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                HtInput::instance.SetControllerButton(m_selectedInputRow, -1);
                m_waitingForInputChoice = false;
            }
            else
            {
                // Check all buttons
                for (int i = 0; i < 255; i++)
                {
                    if (HtGameController::instance.IsNewButtonDown(i))
                    {
                        HtInput::instance.SetControllerButton(m_selectedInputRow, i);
                        m_waitingForInputChoice = false;
                    }
                }
            }
        }
        else    // Shouldn't happen
        {
            m_waitingForInputChoice = false;
        }
    }
    else if(numCommands > 0)            // Selector box is not active. It can move around
    {
        HtGraphics::instance.FillRect(selector, HtGraphics::GREY);
        if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_LEFT) && m_selectedInputColumn > 0)
            m_selectedInputColumn--;
        if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_RIGHT) && m_selectedInputColumn < 2)
            m_selectedInputColumn++;
        if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_UP) && m_selectedInputRow > 0)
            m_selectedInputRow--;
        if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_DOWN) && m_selectedInputRow < numCommands-1)
            m_selectedInputRow++;
        if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_RETURN))
            m_waitingForInputChoice = true;

    }

    // Allo input table to scroll if more than 10 commands
    if (m_selectedInputRow - m_inputTableTop > 9)
        m_inputTableTop++;
    if (m_inputTableTop>0 && (m_selectedInputRow -m_inputTableTop  < 3))
        m_inputTableTop--;

    // Give visual cue if top of table is not zero
    if (m_inputTableTop > 0)
    {
        HtGraphics::instance.WriteTextAligned(Vector2D(-950, 440), "More...", HtGraphics::GREY, 0, 1.5);
    }

    // Display the input table
    for (int i = m_inputTableTop; i < numCommands; i++)
    {
        std::string name = HtInput::instance.GetName(i);
        std::string key = HtInput::GetScancodeName(HtInput::instance.GetKey(i));
        std::string mouse = "";
        if (HtInput::instance.GetMouseButton(i) == HtMouseButton::LEFT) mouse = "LEFT";
        if (HtInput::instance.GetMouseButton(i) == HtMouseButton::MIDDLE) mouse = "MIDDLE";
        if (HtInput::instance.GetMouseButton(i) == HtMouseButton::RIGHT) mouse = "RIGHT";

        int jbutton = HtInput::instance.GetControllerButton(i);
        std::string jbut = "";
        if(jbutton>=0) jbut="Button " + std::to_string(jbutton+1);

        if (i - m_inputTableTop < 12)
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-1000, 350 - (i - m_inputTableTop) * 75), name, HtGraphics::GREY, 0, 1.5);
            HtGraphics::instance.WriteTextAligned(Vector2D(-300, 350 - (i - m_inputTableTop) * 75), key, HtGraphics::WHITE, 0, 1.5);
            HtGraphics::instance.WriteTextAligned(Vector2D(100, 350 - (i - m_inputTableTop) * 75), mouse, HtGraphics::WHITE, 0, 1.5);
            HtGraphics::instance.WriteTextAligned(Vector2D(500, 350 - (i - m_inputTableTop) * 75), jbut, HtGraphics::WHITE, 0, 1.5);
        }
        else
        {
            HtGraphics::instance.WriteTextAligned(Vector2D(-950, 350 - 12 * 75), "More...", HtGraphics::GREY, 0, 1.5);
        }
    }
}






