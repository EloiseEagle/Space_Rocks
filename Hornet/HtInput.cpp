#include "HtInput.h"
#include "ErrorLogger.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>


HtInput HtInput::instance;
const std::string HtInput::KEYFILENAME = "controls.ini";    // Name of the input settings file


HtInput::InputCommand::InputCommand(std::string name, int category)
{
    key = SDL_SCANCODE_UNKNOWN;                 
    pKeyboardListener=nullptr;

    mouseButton=HtMouseButton::NONE;
    pMouseListener = nullptr;

    controllerButton=-1;         
    controllerAxis=-1;           
    controllerAxisThreshold=25;  

    pressed = false;

    this->name = name;
    this->category = category;

    active = true;
}

const char* HtInput::InputCommand::GetKeyName()
{
        return SDL_GetScancodeName(key);
}

HtInput::~HtInput()
{
    RemoveAllCommands();
}

HtInput::HtInput()
{
}

const char* HtInput::GetScancodeName(SDL_Scancode key)
{
    return SDL_GetScancodeName(key);
}

bool HtInput::LoadInputFile()
{
    std::ifstream infile;
    infile.open(KEYFILENAME);

    if (!infile)
        return false;
    else
    {
        while (infile)
        {
            // Read the next line from the file
            std::string line;
            std::getline(infile, line, '\n');

            // Remove spaces
            line.erase(remove(line.begin(), line.end(), ' '));

            // Attempt to tokenise on tab
            std::vector<std::string> tokens;
            std::stringstream linestream(line);
            std::string nextToken;
            while (std::getline(linestream, nextToken, '\t'))
            {
                if (nextToken.substr(0, 2) != "\\\\")
                {
                    tokens.push_back(nextToken);
                }
            }

            if (tokens.size() < 1)
            {
                // Silently ignore the line
            }
            else if (tokens.size() < 6) // Should be six tokens in a valid line
            {
                ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts "+tokens[0]);
            }
            else        // Okay. Read the line
            {
                // Get the data
                std::string name = tokens[0];
                std::string scancode = tokens[1];
                std::string mousebutton = tokens[2];
                std::string controllerbutton = tokens[3];
                std::string controlleraxis = tokens[4];
                std::string controllerthreshold = tokens[5];
                tokens.clear();

                // Add a new command
                CommandIndex newCommand = HtInput::instance.AddCommand(name);

                // Handle the scancode
                try
                {
                    SDL_Scancode sc = (SDL_Scancode)std::stoi(scancode);
                    HtInput::instance.SetKey(newCommand, sc);
                }
                catch (std::invalid_argument)
                {
                    ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts " + tokens[0]);
                    HtInput::instance.SetKey(newCommand, SDL_SCANCODE_UNKNOWN);
                }

                // Handle the mouse button
                if (mousebutton == "MOUSE_NONE")
                    HtInput::instance.SetMouseButton(newCommand, HtMouseButton::NONE);
                else if (mousebutton == "MOUSE_LEFT")
                    HtInput::instance.SetMouseButton(newCommand, HtMouseButton::LEFT);
                else if (mousebutton == "MOUSE_RIGHT")
                    HtInput::instance.SetMouseButton(newCommand, HtMouseButton::RIGHT);
                else if (mousebutton == "MOUSE_MIDDLE")
                    HtInput::instance.SetMouseButton(newCommand, HtMouseButton::MIDDLE);
                else
                {
                    ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts " + tokens[0]);
                    HtInput::instance.SetMouseButton(newCommand, HtMouseButton::NONE);
                }

                // Handle the controller button number
                try
                {
                    int cb = (SDL_Scancode)std::stoi(controllerbutton);
                    HtInput::instance.SetControllerButton(newCommand, cb);
                }
                catch (std::invalid_argument)
                {
                    ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts " + tokens[0]);
                    HtInput::instance.SetControllerButton(newCommand, -1);
                }

                // Handle the controller axis
                try
                {
                    int ca = (SDL_Scancode)std::stoi(controllerbutton);
                    HtInput::instance.SetControllerAxis(newCommand, ca);
                }
                catch (std::invalid_argument)
                {
                    ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts " + tokens[0]);
                    HtInput::instance.SetControllerAxis(newCommand, -1);
                }

                // Handle the controller threshold
                try
                {
                    int ct = (SDL_Scancode)std::stoi(controllerthreshold);
                    HtInput::instance.SetControllerAxis(newCommand, ct);
                }
                catch (std::invalid_argument)
                {
                    ErrorLogger::Write("Bad format in " + KEYFILENAME + " line starts " + tokens[0]);
                    HtInput::instance.SetControllerAxis(newCommand, 25);
                }
            }
        }
    }


    infile.close();

    return true;
}

void HtInput::SaveInputFile()
{
    std::ofstream outfile;
    outfile.open(KEYFILENAME);

    int numkeys = HtInput::instance.GetNumCommands();

    // PUt some instructions in the file
    outfile << "\\\\ use double backslash for comments\n";
    outfile << "\\\\ tabs between fields are important. Don't use spaces\n";
    outfile << "\\\\ Fields are name, scancode number, mouse, controller button, controller axis, axis threshold\n\n";

    // Loop through all commands
    for (int i = 0; i < numkeys; i++)
    {
        // Print the name of the command
        outfile << HtInput::instance.GetName(i) << "\t";

        // Print the scancode as integer of the command
        int keycode = (HtInput::instance.GetKey(i));
        outfile << keycode << "\t";

        // Print the mouse button used
        HtMouseButton mbutton = HtInput::instance.GetMouseButton(i);
        if (mbutton == HtMouseButton::LEFT)
            outfile << "MOUSE_LEFT" << "\t";
        else if (mbutton == HtMouseButton::RIGHT)
            outfile << "MOUSE_RIGHT" << "\t";
        else if (mbutton == HtMouseButton::MIDDLE)
            outfile << "MOUSE_MIDDLE" << "\t";
        else
            outfile << "MOUSE_NONE" << "\t";

        // Print the controller button used
        int jbutton = HtInput::instance.GetControllerButton(i);
        outfile << jbutton << "\t";

        // Print the axis used
        int axis = HtInput::instance.GetControllerAxis(i);
        outfile << axis << "\t";

        // Print the axis threshold
        int threshold = HtInput::instance.GetControllerThreshold(i);
        outfile << threshold << "\t\n";
    }
    outfile.close();
}

CommandIndex HtInput::AddCommand(std::string name)
{
    // Convert to upper case
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

    int newCommand = 0;
    for (const InputCommand& ic : commandList)
    {
        if (ic.name == name)
        {
            ErrorLogger::Write("Command name already in use:" + name);
            return newCommand;
        }
        ++newCommand;
    }

    InputCommand newic(name);
    commandList.push_back(newic);
    return  (int)commandList.size() - 1;
}

CommandIndex HtInput::FindCommand(std::string name) const
{
    // Convert to upper case
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

    int pos = 0;
    for (const InputCommand& ic : commandList)
    {
        if (ic.name == name)
            return pos;
        ++pos;
    }
    ErrorLogger::Write("Command not found: " + name);
    return -1;
}

int HtInput::GetNumCommands() const
{
    return (int)(commandList.size());
}

void HtInput::SetKey(CommandIndex command, SDL_Scancode key)
{
    if(command>=0 && command<commandList.size())
        commandList[command].key = key;
}

void HtInput::SetMouseButton(CommandIndex command, HtMouseButton button)
{
    if (command >= 0 && command < commandList.size())
        commandList[command].mouseButton = button;
}

void HtInput::SetControllerButton(CommandIndex command, int button)
{
    if (command >= 0 && command < commandList.size())
    commandList[command].controllerButton = button;
}

void HtInput::SetControllerAxis(CommandIndex command, int axis, int threshold)
{
    if (command >= 0 && command < commandList.size())
    {
        commandList[command].controllerAxis = axis;
        commandList[command].controllerAxisThreshold = threshold;
    }
}

SDL_Scancode HtInput::GetKey(CommandIndex command) const
{
    if (command < 0 || command >= commandList.size())
    {
        return SDL_SCANCODE_UNKNOWN;
    }
    else
    {
        return commandList[command].key;
    }
    return SDL_SCANCODE_UNKNOWN;
}

std::string HtInput::GetName(CommandIndex command) const
{
    if (command < 0 || command >= commandList.size())
    {
        return "";
    }
    else
    {
        return commandList[command].name;
    }
}

HtMouseButton HtInput::GetMouseButton(CommandIndex command) const
{
    if (command < 0 || command >= commandList.size())
    {
        return HtMouseButton::NONE;
    }
    else
    {
        return commandList[command].mouseButton;
    }
}

int HtInput::GetControllerButton(CommandIndex command) const
{
    if (command < 0 || command >= commandList.size())
    {
        return -1;
    }
    else
    {
        return commandList[command].controllerButton;
    }
}

int HtInput::GetControllerAxis(CommandIndex command)
{
    if (command < 0 || command >= commandList.size())
    {
        return -1;
    }
    else
    {
        return commandList[command].controllerAxis;
    }
}

int HtInput::GetControllerThreshold(CommandIndex command)
{
    if (command < 0 || command >= commandList.size())
    {
        return -1;
    }
    else
    {
        return commandList[command].controllerAxisThreshold;
    }
}

void HtInput::Disable(int category)
{
    for (InputCommand& command : commandList)
    {
        if (command.category == category)
        {
            command.active = false;
        }
    }
}

void HtInput::Enable(int category)
{
    for (InputCommand& command : commandList)
    {
        if (command.category == category)
        {
            command.active = true;
        }
    }
}



bool HtInput::Pressed(CommandIndex command)
{
    if (command >= 0 && command < commandList.size())
    {
        if (HtKeyboard::instance.KeyPressed(commandList[command].key))
        {
            return true;
        }
        if (HtMouse::instance.IsMouseDown(commandList[command].mouseButton))
        {
            return true;
        }
        if (HtGameController::instance.IsButtonDown(commandList[command].controllerButton))
        {
            return true;
        }
        if (commandList[command].controllerAxis > -1)                 // May need to address this for negative values. I'll think about it.
        {
            int threshold = commandList[command].controllerAxisThreshold;
            int value = HtGameController::instance.GetAxis(commandList[command].controllerAxis);
            if (value > threshold || value < -threshold)
                return true;
        }
    }

    return false;
}

bool HtInput::NewPressed(CommandIndex command)
{
    if (command > 0 && command < commandList.size())
    {
        if (Pressed(command))
        {
            if (commandList[command].pressed == false)
            {
                commandList[command].pressed = true;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            commandList[command].pressed = false;
            return false;
        }
    }
    return false;
}

void HtInput::RemoveAllCommands()
{
    commandList.clear();
    commandList.shrink_to_fit();    // Avoids a memory leak. Only really matters for leak reporting.
}


