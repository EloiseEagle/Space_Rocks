#include "HtKeyboard.h"

HtKeyboard HtKeyboard::instance;

HtKeyboard::HtKeyboard()
{
    m_textInput="";

    // If true, system is accepting new input to the text input string.
    // If false, key input events will be ignored
    m_takingTextInput= false;

    m_pListener = nullptr;
    m_pPreviousStates = nullptr;
    m_previousStatesArraySize = 0;

    m_pListener = nullptr;
}

HtKeyboard::~HtKeyboard()
{
    // In case this didn't happen
    Shutdown();
}

Result HtKeyboard::Initialise()
{
    // This gets the size of the array
    SDL_GetKeyboardState(&m_previousStatesArraySize)[0];
    if (m_previousStatesArraySize > 0)
    {
        // Creating an array to track previous keyboard state
        // used in NewKeyPressed
        m_pPreviousStates = new bool[m_previousStatesArraySize];

        // Set all keys to "currently pressed" so NewKeyPressed
        // returns false on first call for that key
        for (int i = 0; i < m_previousStatesArraySize; ++i)
        {
            m_pPreviousStates[i] = true;
        }
        return Result::SUCCESS;
    }
    else
    {
        m_pPreviousStates = nullptr;
        m_previousStatesArraySize = 0;
        return Result::FAILURE;
    }
}

bool HtKeyboard::KeyPressed(SDL_Scancode SDLkey)
{
    // The NULL allows me to return by pointer the size of the array.
    // I don't care.
    return SDL_GetKeyboardState(NULL)[SDLkey];
}

bool HtKeyboard::NewKeyPressed(SDL_Scancode SDLkey)
{
    bool pressed = false;

    // If there is no valid array for some reason, just return false
    if (m_pPreviousStates && SDLkey < m_previousStatesArraySize && SDLkey>=0)
    {
        // Is the key pressed right now?
        bool current = SDL_GetKeyboardState(NULL)[SDLkey];

        if(current && !m_pPreviousStates[SDLkey])
        {
            pressed = true;
        }

        // Remember the state of this key
        m_pPreviousStates[SDLkey] = current;

    }
    return pressed;

}

void HtKeyboard::StartTextInput()
{
    // I have heard a rumor that you don't actually need this for
    // Text input events, only for compositing
    SDL_StartTextInput();

    m_textInput = "";

    m_takingTextInput = true;
}


std::string HtKeyboard::StopTextInput()
{
    SDL_StopTextInput();
    m_takingTextInput = false;
    return m_textInput;
}

std::string HtKeyboard::PeekTextInput()const
{
    return m_textInput;
}

void HtKeyboard::RegisterKeyboardListener(IHtKeyboardListener* pListener)
{
    m_pListener = pListener;
}

void HtKeyboard::DeRegisterKeyboardListener()
{
    m_pListener = nullptr;
}

void HtKeyboard::HandleKeyEvent(const SDL_Event& evt)
{
    if (m_pListener)
    {
        KeyboardEventType type = KeyboardEventType::NONE;
        if(evt.type==SDL_KEYDOWN) type = KeyboardEventType::PRESSED;
        else if (evt.type == SDL_KEYUP)
        {
            type = KeyboardEventType::RELEASED;
        }
        else if (evt.type == SDL_KEYDOWN)
        {
            if (evt.key.repeat == 0)
                type = KeyboardEventType::PRESSED;
            else
                type = KeyboardEventType::REPEAT;
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_G)
            int x = 0;
        m_pListener->HandleKeyboardEvent(evt.key.keysym.scancode, type);
    }
}

void HtKeyboard::AddTextInput(char* text)
{
    if (m_takingTextInput)
    {
        m_textInput += text;
    }
}

void HtKeyboard::Shutdown()
{
    // Array delete the array of keyboard states
    delete[] m_pPreviousStates;
    m_pPreviousStates = nullptr;
}
