#pragma once
#include <string>
#include "SDL.h"
#include "Result.h"

class HtGameController
{
public:
    // Releases the current controller.
    void Release();

    Result Initialise();

    // Returns the current position of the X axis.
    // Return value varies from -127 (left) to +128 (right) with zero as the centre position
    int GetXAxis()const;

    // Returns the current position of the Y axis.
    // Return value varies from -127 (down) to +128 (up) with zero as the centre position
    int GetYAxis()const;

    // Returns the current position of the Z axis.
    // What the "z" axis actually means is dependent on the device
    int GetZAxis()const;

    // Returns the current position of the specified axis.
    // 0 is normally axis X and 1 is normally axis Y
    // Other axes are dependent on the device
    // Return value varies from -127 to +128 with zero as the centre position
    int GetAxis(int axisNumber)const;

    // Returns the number of axes on the current controller, if the
    // driver supports this request.
    // If no controller is connected, returns 0.
    int GetNumAxes()const;

    // Returns the name of the current controller or an empty string if controller
    // is not connected
    std::string GetControllerName();

    // Sets the current controller. If there is only one controller, this will be number 0.
    // If there are more controllers, then you can try larger numbers, from 1 upwards. 
    // Returns true if the requested number connects to a controller. 
    // false otherwise.
    // If you connnect to an unavalable controller, the controller is now "unavailable".
    // See GetNumControllers()
    bool SetCurrentController(int controllerNumber);

    // Returns the number of buttons on the current controller, if the
    // driver supports this request.
    // If no controller is connected, returns 0.
    int GetNumButtons()const;

    // Returns true if the specified button button is currently down.
    // Returns false if not down, or no controller is connected, or specified button
    // is not available.
    // Button numbers start at zero and count upwards. So if your controller has four buttons
    // they will be numbered 0-3
    bool IsButtonDown(int buttonNumber)const;

    // Returns true if the specified button button has been pressed
    // since the last time you called this function for this specific button.
    bool IsNewButtonDown(int buttonNumber);

    // Returns true if a controller is currently available.
    // Returns false if no controller is connected, or if you have tried to use
    // SetCurrentController( ) for a non-existent controller
    bool IsControllerAvailable() const;

    // Returns the number of game controllers attached to the system
    int GetNumControllers() const;

    static HtGameController instance;

private:
    HtGameController();
    HtGameController(const HtGameController& other)=delete;
    ~HtGameController();

    int m_controllerNumber;
    static const int NUMTRACKEDBUTTONS = 12;
    bool m_buttonTrack[NUMTRACKEDBUTTONS];
    SDL_Joystick* m_pJoystick;
    bool m_available;

};

