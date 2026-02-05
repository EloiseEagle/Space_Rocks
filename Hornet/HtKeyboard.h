#pragma once
#include "SDL.h"
#include <string>
#include <map>
#include "Result.h"

// Chris Rook c.rook@northumbria.ac.uk
// 5/5/2023 First stable version

enum class KeyboardEventType{PRESSED, RELEASED, REPEAT, NONE};

// The keyboard listener interface. Inherit from this to make any class an IHtKeyboardListener.
// You will need to implement the abstract function in that class.
// An object of that class can be regstered with HtKeyboard as a KeyboardListener
// to receive key press event calls.
class IHtKeyboardListener
{
public:
    // Receives keyboard events if registered with HtKeyboard.
    // SDLKey is which key is pressed or released. See KeyPressed() for possible values.
    // keyboardEventType is KeyboardEventType::PRESSED if the key is pressed.
    // keyboardEventType is KeyboardEventType::RELEASED if the key is released.
    // keyboardEventType is KeyboardEventType::REPEAT if the key has triggered a Repeat keystroke
    // (i.e. has been held down for a while)
    virtual void HandleKeyboardEvent(const SDL_Scancode& SDLkey, KeyboardEventType keyboardEventType) = 0;
};



// This class is a wrapper for SDL2 functions and supports operations using a keyboard
// The class supports:
//      polling for current key state (see KeyPressed)
//      callbacks for key events (see RegisterKeyboardEventFunction)
//      text input (See StartTextInput, StopTextInput and PeekTextInput)
class HtKeyboard
{
public:
    // Single instance for the class. Using the Singleton pattern
    static HtKeyboard instance;

    Result Initialise();

    // Returns true if the indicated key is currently pressed.
    // False otherwise.
    // key can be:
    //          Numbers at top of keyboard: SDL_SCANCODE_0 to SDL_SCANCODE_9
    //          Function keys: SDL_SCANCODE_F1 to SDL_SCANCODE_F24
    //          Letters: SDL_SCANCODE_A to SDL_SCANCODE_Z
    //          Others: Try values like SDL_SCANCODE_ESCAPE, SDL_SCANCODE_SPACE, SDL_SCANCODE_RETURN
    //              SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD, SDL_SCANCODE_LSHIFT and so on.
    //          Keypad: SDL_SCANCODE_KP_0 to SDL_SCANCODE_KP_9. Also SDL_SCANCODE_KP_PLUS and so on.
    bool KeyPressed(SDL_Scancode SDLkey);

    // Returns true if the indicated key is currently pressed, but was not 
    // press the last time this function was called (for this specific key)
    // False otherwise.
    bool NewKeyPressed(SDL_Scancode SDLkey);

    // Use this to start text input when you want the player to type a string of letters.
    // This function resets the text input string to "".
    void StartTextInput();

    // This function stops the class from handling text input, and also
    // returns the string of input that has been typed. 
    // If you don't want to stop text input, see PeekTextInput()
    // The function does not clear the string already received, so PeekTextInput will
    // still work after calling this function.
    // To clear the string, use StartTextInput
    std::string StopTextInput();

    // Returns the string of input that has been typed so far.
    // This assumes that StartTextInput() has been called. 
    // This function will still return a value after StopTextInput()
    // The string gets reset to "" when StartTextInput is called again
    std::string PeekTextInput() const;

    // Registers a class to receive keyboard events.
    // The system only tracks one such class, but the object can be changed by
    // calling this function again.
    void RegisterKeyboardListener(IHtKeyboardListener *pListener);

    // This removes the current listener.
    void DeRegisterKeyboardListener();

    // This function is called by the game engine to inform the system of key events.
    // There is no reason to use this function for other purposes
    void HandleKeyEvent(const SDL_Event& evt);

    // This function is called by the game engine to inform the system of text input events
    // There is no reason to use this function for other purposes
    void AddTextInput(char* text);

    // Called by HornetApp at the end of the program to delete dynamically assigned memory
    void Shutdown();

private:

    // This is a pointer to the current keyboard listener, if any.
    IHtKeyboardListener* m_pListener;

    // Constructor is private, because this is a singleton
    HtKeyboard();

    // Disable the copy constructor, because this is a singleton
    HtKeyboard(HtKeyboard& other) = delete;

    // Destructor is private, because this is a singleton
    ~HtKeyboard();

    // The current string of text input from the user
    std::string m_textInput; 

    // If true, system is accepting new input to the text input string.
    // If false, key input events will be ignored
    bool m_takingTextInput;

    // Pointer to the array of keyboard states. Used by NewKeyPressed
    // to keep track of whether a key was previously pressed.
    bool* m_pPreviousStates;
    int m_previousStatesArraySize;
};

