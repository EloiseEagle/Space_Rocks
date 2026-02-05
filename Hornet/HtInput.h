// 16/09/2025   Class created


#pragma once
#include "HtKeyboard.h"
#include "HtMouse.h"
#include "HtGameController.h"
#include <vector>

// The input listener interface. Inherit from this to make any class an InputListener.
// You will need to implement the two abstract functions in that class.
// An object of that class can be regstered with HtInputs as n InputListener
// to receive input event calls.
class IHtInputListener
{
public:
    // Called when the input is activated
    virtual void HandleInputActive(/*Probably parameter. Which input?*/) = 0;

    // Called when the input is activated
    virtual void HandleInputReleased(/*Probably parameter. Which input?*/) = 0;
};



typedef int CommandIndex;

class HtInput
{
private:

    struct InputCommand
    {
        bool active;

        SDL_Scancode key;                   // SDL_Scancode. UNKNOWN if not assigned
        IHtKeyboardListener* pKeyboardListener;

        HtMouseButton mouseButton;          // NONE, LEFT, RIGHT, MIDDLE
        IHtMouseListener* pMouseListener;

        int controllerButton;               // 0-???            -1 if not used
        int controllerAxis;                 // 0-???            -1 if not used
        int controllerAxisThreshold;        // Percent

        std::string name;

        bool pressed;                       // True if pressed on previous call to NewPressed. False otherwise.

        // Give the input a name, and optionally a category number (default category is zero)
        // A category may be useful if you want to disable or enable a group of commands
        InputCommand(std::string name, int category = 0);

        int category;                       // Default zero. Used for putting them into a menu, or for disabling a category

        const char* GetKeyName();
    };

    std::vector<InputCommand> commandList;

    ~HtInput();
    HtInput();
    HtInput(HtInput&) = delete;
    static const std::string KEYFILENAME;

public:
    // Returns a human-readable name for the specified scancode
    static const char* GetScancodeName(SDL_Scancode key);

    static HtInput instance;           // Yes, it's a singleton. Cry about it.

    // Loads the file (with name in KEYFILENAME) containing a list of InputCommands
    bool LoadInputFile();

    // Saves the file (with name in KEYFILENAME) containing a list of InputCommands
    void SaveInputFile();

    // Adds a new command with the given name and returns the index of the new command
    // If the name already exists, returns the index of the existing command, and does not add a new one
    CommandIndex AddCommand(std::string name);

    // Returns the index of the command with the given name (case-insensitive comparison)
    // If no command with that name can be found returns -1.
    CommandIndex FindCommand(std::string name) const;

    // Returns the number of input commands currently registered
    int GetNumCommands()const;

    // Sets a keyboard key for this command (replaces any previous key)
    void SetKey(CommandIndex command, SDL_Scancode key);

    // Sets a mouse button for this command (replaces previous button)
    void SetMouseButton(CommandIndex command, HtMouseButton button);

    // Sets a controller button for this command (replaces any previous controller button)
    void SetControllerButton(CommandIndex command, int button);

    // Specifies a controller axis for this command. Threshold is percentage, default 25
    void SetControllerAxis(CommandIndex command, int axis, int threshold = 25);

    // Returns the keyboard key for this command. UNKNOWN if not assigned
    SDL_Scancode GetKey(CommandIndex command) const;

    // Returns the name for this command. "" if not assigned
    std::string GetName(CommandIndex command) const;

    // Gets the mouse button for this command
    HtMouseButton GetMouseButton(CommandIndex command)const;

    // Gets a controller button for this command (-1 if not assigned)
    int GetControllerButton(CommandIndex command)const;

    // Gets the controller axis for this command. -1 if not assigned.
    int GetControllerAxis(CommandIndex command);

    // Gets the controller axis for this command. -1 if not assigned.
    int GetControllerThreshold(CommandIndex command);

    // Disables all commands with this category number, without removing them from the list.
    // Call Enable to make it active again.
    void Disable(int category);

    // Disables all commands with this category number, without removing them from the list.
    // Call Reenable to make it active again.
    void Enable(int category);

    // Returns false if not pressed, a true if pressed 
    // (for joystick axis, returns true if value is above the threshold)
    bool Pressed(CommandIndex command);

    // Returns true if pressed, but was not pressed on previous call for this input
    bool NewPressed(CommandIndex command);

    // Deletes all input commands from the system
    void RemoveAllCommands();

    // Not implemented
    void AddInputListener(CommandIndex command, IHtInputListener* pListener);

    // Not implemented
    void RemoveInputListener(IHtInputListener* pListener);


};