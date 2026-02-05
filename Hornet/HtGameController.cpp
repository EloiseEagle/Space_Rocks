#include "HtGameController.h"


HtGameController HtGameController::instance;

void HtGameController::Release()
{
    SDL_JoystickClose(m_pJoystick);
    m_pJoystick = nullptr;
    m_available = false;
}

Result HtGameController::Initialise()
{
    m_available = SetCurrentController(0);
    // Fail silently. Systems often don't have a joystick.

    if (m_available) return Result::SUCCESS;
    else return Result::FAILURE;
}

int HtGameController::GetXAxis()const
{
    return SDL_JoystickGetAxis(m_pJoystick, 0) >> 8;
}

int HtGameController::GetYAxis()const
{
    return SDL_JoystickGetAxis(m_pJoystick, 1) >> 8;
}

int HtGameController::GetZAxis()const
{
    return SDL_JoystickGetAxis(m_pJoystick, 1) >> 8;
}

int HtGameController::GetAxis(int axisNumber)const
{
    return SDL_JoystickGetAxis(m_pJoystick, axisNumber) >> 8;
}

int HtGameController::GetNumAxes() const
{
    if (m_pJoystick)
        return SDL_JoystickNumAxes(m_pJoystick);
    else
        return 0;
}

std::string HtGameController::GetControllerName()
{
    if (m_pJoystick)
    {
        return  SDL_JoystickName(m_pJoystick);
    }
    else
    {
        return "";
    }

}

bool HtGameController::SetCurrentController(int controllerNumber)
{
    SDL_JoystickClose(m_pJoystick);
    m_pJoystick = nullptr;
    m_controllerNumber = controllerNumber;
    m_pJoystick = SDL_JoystickOpen(m_controllerNumber);
    if (m_pJoystick)
    {
        m_available = true;
    }
    else
    {
        m_available = false;
    }

    return (m_pJoystick != nullptr);
}

int HtGameController::GetNumButtons() const
{
    if (m_pJoystick)
        return SDL_JoystickNumButtons(m_pJoystick);
    else
        return 0;
}

bool HtGameController::IsButtonDown(int buttonNumber)const
{
    return SDL_JoystickGetButton(m_pJoystick, buttonNumber)==1;
}

bool HtGameController::IsNewButtonDown(int buttonNumber)
{
    // Requesting a button outside the range of tracked buttons
    if (buttonNumber < 0 || buttonNumber >= NUMTRACKEDBUTTONS)
    {
        return false;
    }
    // If that button is currently pressed
    else if (SDL_JoystickGetButton(m_pJoystick, buttonNumber) == 1)
    {
        if (m_buttonTrack[buttonNumber])
        {
            return false;
        }
        else
        {
            m_buttonTrack[buttonNumber] = true;
            return true;
        }
    }
    else 
    {
        m_buttonTrack[buttonNumber] = false;
        return false;
    }
}


bool HtGameController::IsControllerAvailable() const
{
    return m_available;
}

int HtGameController::GetNumControllers() const
{
    return SDL_NumJoysticks();
}

HtGameController::HtGameController()
{
    for (int i = 0; i < NUMTRACKEDBUTTONS; ++i)
    {
        m_buttonTrack[i] = false;
    }

    m_available = false;
    m_controllerNumber = 0;
    m_pJoystick = nullptr;


}

HtGameController::~HtGameController()
{
    Release();
}
