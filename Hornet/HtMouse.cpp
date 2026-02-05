#include "HtMouse.h"
#include "ErrorLogger.h"
#include <SDL.h>
#include "HtCamera.h"

HtMouse HtMouse::instance;

Result HtMouse::SetPointerVisiblity(bool visible)
{

    SDL_bool flag = SDL_TRUE;
    if (visible)
        flag = SDL_FALSE;
    if (SDL_SetRelativeMouseMode(flag) < 0)
    {
        ErrorLogger::Write("Failed to hide or show curser in SetCursorVisiblity().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

Result HtMouse::Initialise()
{
    SDL_GetMouseState(&m_pointerX, &m_pointerY);
    m_mouseDX = 0;
    m_mouseDY = 0;
    m_mouseDZ = 0;
    return Result::SUCCESS;
}

int HtMouse::GetMouseMoveX()
{
    int answer = m_mouseDX;
    m_mouseDX = 0;
    return answer;
}

int HtMouse::GetMouseMoveY()
{
    int answer = m_mouseDY;
    m_mouseDY = 0;
    return answer;
}

int HtMouse::GetMouseMoveZ()
{
    int answer = m_mouseDZ;
    m_mouseDZ = 0;
    return answer;
}

bool HtMouse::IsMouseDown(HtMouseButton button) const
{
    if (button == HtMouseButton::LEFT)
        return m_leftDown;
    else if (button == HtMouseButton::MIDDLE)
        return m_middleDown;
    else if (button == HtMouseButton::RIGHT)
        return m_rightDown;
    else if (button == HtMouseButton::NONE)
    {
        return false;
    }
    else
    {
        ErrorLogger::Write("Requesting an invalid mouse button in IsMouseDown().");
        return false;
    }


}

bool HtMouse::IsNewMouseDown(HtMouseButton button)
{
    bool answer = false;
    if (button == HtMouseButton::LEFT)
    {
        if (m_leftDown && !m_oldLeftDown)
        {
            answer = true;
        }
        m_oldLeftDown = m_leftDown;
    }
    else if (button == HtMouseButton::MIDDLE)
    {
        if (m_middleDown && !m_oldMiddleDown)
        {
            answer = true;
        }
        m_oldMiddleDown = m_middleDown;
    }
    else if (button == HtMouseButton::RIGHT)
    {
        if (m_rightDown && !m_oldRightDown)
        {
            answer = true;
        }
        m_oldRightDown = m_rightDown;
    }
    else if (button == HtMouseButton::NONE)
    {
        answer = false;
    }
    else
    {
        ErrorLogger::Write("Requesting an invalid mouse button in IsNewMouseDown().");
        answer = false;
    }
    return answer;
}

Vector2D HtMouse::GetPointerGamePosition()
{
    Vector2D answer(m_pointerX, m_pointerY);
    answer = HtCamera::instance.GameTransform(answer);
    return answer;
}

void HtMouse::SetPointerWorldPosition(Vector2D position)
{
    // To do
}

void HtMouse::SetPointerScreenPosition(Vector2D position)
{
    // To do
}

Result HtMouse::SetPointerPosition(int XPos, int YPos, int ZPos)
{
    m_pointerZ = 0;
    if (SDL_WarpMouseGlobal(XPos, YPos) < 0)
    {
        ErrorLogger::Write("Failed to warp the mouse in SetPointerPosition().");
        ErrorLogger::Write(SDL_GetError());
        return Result::FAILURE;
    }
    return Result::SUCCESS;
}

int HtMouse::GetPointerPositionX() const
{
    return m_pointerX;
}

int HtMouse::GetPointerPositionY() const
{
    return m_pointerY;
}

int HtMouse::GetPointerPositionZ() const
{
    return m_pointerZ;
}

void HtMouse::HandleMouseEvent(SDL_Event &evt)
{
    int buttons = SDL_GetMouseState(&m_pointerX, &m_pointerY);
    m_leftDown = (buttons&(SDL_BUTTON(SDL_BUTTON_LEFT)))>0;
    m_rightDown = (buttons & (SDL_BUTTON(SDL_BUTTON_RIGHT)))>0;
    m_middleDown = (buttons & (SDL_BUTTON(SDL_BUTTON_MIDDLE)))>0;

    if (evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP)
    {
        HtMouseButton button = HtMouseButton::LEFT;
        bool down= false;
        if (evt.button.button == SDL_BUTTON_LEFT)
        {
            button = HtMouseButton::LEFT;
        }
        else if (evt.button.button == SDL_BUTTON_RIGHT)
        {
            button = HtMouseButton::RIGHT;
        }
        else if (evt.button.button == SDL_BUTTON_MIDDLE)
        {
            button = HtMouseButton::MIDDLE;
        }
        if (evt.button.state == SDL_PRESSED)
        {
            down = true;
        }
        else
        {
            down = false;
        }
        if (m_pMouseListener)
        {
            m_pMouseListener->HandleMouseButtonEvent(button, down);
        }
    }
    else if (evt.type == SDL_MOUSEMOTION)
    {
        if (m_pMouseListener)
        {
            m_pMouseListener->HandleMouseMove(m_pointerX, m_pointerY, m_pointerZ);
        }
        m_mouseDX += evt.motion.xrel;
        m_mouseDY += evt.motion.yrel;
    }
    else if (evt.type == SDL_MOUSEWHEEL && evt.wheel.y != 0)
    {
        m_pointerZ += evt.wheel.y;
        m_mouseDZ += evt.wheel.y;
        if (m_pMouseListener)
        {
            m_pMouseListener->HandleMouseMove(m_pointerX, m_pointerY, m_pointerZ);
        }
    }
    
}

void HtMouse::RegisterMouseListener(IHtMouseListener* pListener)
{
    m_pMouseListener = pListener;
}

void HtMouse::DeRegisterMouseListener()
{
    m_pMouseListener = nullptr;
}

HtMouse::HtMouse()
{
    int buttons = SDL_GetMouseState(&m_pointerX, &m_pointerY);
    m_leftDown = (SDL_BUTTON(SDL_BUTTON_LEFT) == 1);
    m_rightDown = (SDL_BUTTON(SDL_BUTTON_RIGHT) == 1);
    m_middleDown = (SDL_BUTTON(SDL_BUTTON_MIDDLE) == 1);
    m_pointerZ=0;
    m_mouseDX = 0;
    m_mouseDY = 0;
    m_mouseDZ = 0;
    m_oldLeftDown= m_leftDown;
    m_oldRightDown= m_rightDown;
    m_oldMiddleDown= m_middleDown;
    m_pMouseListener = nullptr;
}

HtMouse::~HtMouse()
{

}

