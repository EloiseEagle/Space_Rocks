#pragma once
#include "Vector2D.h"
#include "Result.h"
#include <SDL.h>

enum class HtMouseButton{NONE, LEFT,MIDDLE,RIGHT};

// The mouse listener interface. Inherit from this to make any class a MouseListener.
// You will need to implement the two abstract functions in that class.
// An object of that class can be regstered with HtMouse as a MouseListener
// to receive mouse event calls.
class IHtMouseListener
{
public:
    // Called when the mouse moves. Parameters are the new x,y,z values of the
    // mouse position. These are in system coordinates, not game world coordinates.
    virtual void HandleMouseMove(int x, int y, int z)=0;

    // Called when a mouse button is clicked or released. "button" is which button
    // changed. Values are HtMouseButton::LEFT, HtMouseButton::RIGHT and HtMouseButton::MIDDLE
    // down is true if the button was clicked and false if released
    virtual void HandleMouseButtonEvent(HtMouseButton button, bool down) = 0;
};

class HtMouse
{
public:
    // Sets the system pointer to visible or hidden.
    // If the pointer is invisible, mouse movements are not constrained by the edge
    // of the screen, which might be useful for some control purposes. 
    // This means that the mouse pointer (though invisible) is still on the screen
    // but you will still get DeltaX and DeltaY movements when the mouse moves,
    // even if the pointer is at the edge of the screen.
    // If the pointer is visible, you won't get DeltaX and DeltaY if you try to
    // move the pointer beyond the screen limits.
    // Set visible to true to show the system cursor.
    // Set visible to false to hide the system cursor
    Result SetPointerVisiblity(bool visible);

    // Initialises the HtMouse system. Called in HtApp
    Result Initialise();

    // Returns the horizontal movement in pixels that the mouse has moved
    // since this function was last called (or since Initialise()) if this
    // is the first time it is called. Note that this is the movement in screen
    // pixels, not game world units. Positive values indicate a movement to the right.
    // Negative values indicate a movement to the left.
    int GetMouseMoveX();

    // Returns the vertical movement in pixels that the mouse has moved
    // since this function was last called (or since Initialise()) if this
    // is the first time it is called. Note that this is the movement in screen
    // pixels, not game world units. Positive values indicate a movement downwards.
    // Negative values indicate a movement upwards.
    int GetMouseMoveY();

    // Returns the movement of the mouse wheel
    // since this function was last called (or since Initialise()) if this
    // is the first time it is called. Positive values indicate a movement forwards.
    // Negative values indicate a movement backwards.
    int GetMouseMoveZ();

    // Returns true if the specified mouse button is currently held down
    // false otherwise. Buttons are HtMouseButton::LEFT, HtMouseButton::RIGHT
    // and HtMouseButton::MIDDLE.
    bool IsMouseDown(HtMouseButton button) const;

    // Returns true if the specified mouse button is currently held down
    // but was NOT down on the previous call to this function.
    // false otherwise. Buttons are HtMouseButton::LEFT, HtMouseButton::RIGHT
    // and HtMouseButton::MIDDLE.
    // Always returns false on the first call.
    bool IsNewMouseDown(HtMouseButton button);

    // Returns the position vector of the current mouse position in game coordinates,
    // using the current camera settings. If camera is set to inactive, this will
    // the screen position where (0,0) is the centre of the screen. 
    // If the camera is active, this will be the world position, and will depend on the
    // position of the camera
    Vector2D GetPointerGamePosition();


    // Moves the mouse to the specified position in world coordinates.
    void SetPointerWorldPosition(Vector2D position);

    // Moves the mouse to the specified position in screen coordinates.
    void SetPointerScreenPosition(Vector2D position);

    // Moves the mouse to the specified position in screen pixel coordinates.
    Result SetPointerPosition(int XPos, int YPos, int ZPos=0);

    // Returns the X value of the mouse position using screen pixel coordinates.
    // Note that this is not the same as game world coordinates. 
    // The left of ths screen is value 0 and the right will depend on screen resolution.
    int GetPointerPositionX() const;

    // Returns the Y value of the mouse position using screen pixel coordinates.
    // Note that this is not the same as game world coordinates. 
    // The top of ths screen is value 0 and the bottom will depend on screen resolution.
    int GetPointerPositionY() const;

    // Returns the Z value of the mouse position.
    int GetPointerPositionZ() const;

    // This is used by HtApp to report mouse movement events.
    // Don't call it unless you know what you are doing.
    void HandleMouseEvent(SDL_Event &evt);

    // Registers a MouseListenr object to receive mouse movement notifications.
    // Currently only one MouseListener can be registered at a time, so using
    // this will replace any existing MouseListener.
    void RegisterMouseListener(IHtMouseListener* pListener);

    // Deregisters the current mouse listener, if any.
    void DeRegisterMouseListener();

    // Singleton instance
    static HtMouse instance;
private:
    // Private constructor for singleton
    HtMouse();

    // Disabled copy constructor for singleton
    HtMouse(HtMouse& other) = delete;

    // Private destructor for singleton
    ~HtMouse();
    int m_pointerX;     // Mouse position set by last call to HandleMouseEvent
    int m_pointerY;     // Mouse position set by last call to HandleMouseEvent
    int m_pointerZ;     // Mouse position set by last call to HandleMouseWheelEvent
    int m_mouseDX;  // How much the mouse have moved since last GetMouseMoveX
    int m_mouseDY;  // How much the mouse have moved since last GetMouseMoveY
    int m_mouseDZ;  // How much the mouse have moved since last GetMouseMoveZ
    bool m_leftDown;    // If true, left mouse was down on last call to HandleMouseEvent
    bool m_rightDown;   // If true, right mouse was down on last call to HandleMouseEvent
    bool m_middleDown;   // If true, middle mouse was down on last call to HandleMouseEvent
    bool m_oldLeftDown; // Left mouse position on last call to IsNewMouseDown
    bool m_oldRightDown; // Right mouse position on last call to IsNewMouseDown
    bool m_oldMiddleDown; // Middle mouse position on last call to IsNewMouseDown

    IHtMouseListener* m_pMouseListener; // Currently registered mouse listener
};

