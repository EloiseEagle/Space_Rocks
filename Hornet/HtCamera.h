#pragma once
#include "vector2D.h"
#include "Shapes.h"

// The camera class is used to transform world coordinates to/from screen coordinates
// Can also transform Cirle2D, Rectangle2D and Segment2D objects
class HtCamera
{
public:
	// Sets the camera with an intial position at 0,0 with 
	// a zoom scale to make the screen have a height of 2000 using world coordinates.
	// So the top of the screen is 1000. Bottom of the screen is -1000.
	// Left and right will depend on screen aspect, but for a typical wide screen
	// monitor is around -1775 to + 1775
	void Reset();

	// Places the centre of the camera to the specified position
	// The default position is (0,0)
	void PlaceAt(Vector2D worldPosition);

	// Returns the current world position of the centre of the screen.
	// The default position is (0,0)
	Vector2D GetCameraCentre(Vector2D worldPosition);

	// Sets the camera zoom. A zoom of 1.0 produces a screen height of 2000 world
	// units. The width depends on the screen aspect ratio. For 1600x900 resolution,
	// the width should be about 3555 for a zoom of 1.0
	void SetZoom(double zoom);

	// Returns the camera zoom. A zoom of 1.0 produces a screen height of 2000 world
	// units.
	double GetZoom() const;

	// Returns the area of the world that is on screen, given the current camera
	// position and zoom. Any shape that collides with this is at least partially on-screen
	Rectangle2D GetCameraArea() const;

	// Returns the Y-Axis value of the top of the screen in world coordinates,
	// given the current camera position and zoom.
	double GetTopOfCameraArea() const;

	// Returns the Y-Axis value of the bottom of the screen in world coordinates,
	// given the current camera position and zoom.
	double GetBottomOfCameraArea() const;

	// Returns the X-Axis value of the left side of the screen in world coordinates,
	// given the current camera position and zoom.
	double GetLeftOfCameraArea() const;

	// Returns the X-Axis value of the right side of the screen in world coordinates,
	// given the current camera position and zoom.
	double GetRightOfCameraArea() const;

	// Transforms a game size to a native size
	// using current camera settings
	double NativeTransform(double gameSize);

	// Transforms a game position vector to a native position vector
	// using current camera settings
	Vector2D NativeTransform(Vector2D gamePosition);

	// Transforms a game circle2D to a native  Circle2D
	// using current camera settings
	Circle2D NativeTransform(Circle2D gameCircle);

	// Transforms a game Rectangle2D to a native Rectangle2D
	// using current camera settings
	Rectangle2D NativeTransform(Rectangle2D gameRectangle);

	// Transforms a game Rectangle2D to a native Rectangle2D
	// using current camera settings
	AngledRectangle2D NativeTransform(AngledRectangle2D gameRectangle);

	// Transforms a game Segment2D to a native Segment2D
	// using current camera settings
	Segment2D NativeTransform(Segment2D gameSegment);

	// Transforms a native size to a game size
	// using current camera settings
	double GameTransform(double nativeSize);

	// Transforms a native position vector to a game position vector
	// using current camera settings
	Vector2D GameTransform(Vector2D nativePosition);

	// Enables or disables the camera. If set to true, objects will be displayed
	// relative to the camera position. If set to false, objects will be displayed
	// as if the camera were set to 0,0 with a zoom of 1.
	// Turn camera on to display in-game objects. Turn camera off for HUD elements.
	void UseCamera(bool cameraUsed);

	// Returns true if the specified position vector is currently in the camera's viewport
	// False otherwise
	// Note that this uses the camera's position, not the default position, so if you
	// have called UseCamera(false) the function is reporting on whether the point is
	// visible from where the camera should be, not whether it is on screen
	bool IsOnCamera(Vector2D point) const;

	// Returns true if the camera is currently active.
	// false otherwise
	bool IsCameraActive() const;


	static HtCamera instance;

private:
	// By default the camera will put the centre of the screen at (0,0) with 
	// a scale to make the screen have a height of 2000 using world coordinates.
	// So the top of the screen is 1000. Bottom of the screen is -1000.
	// Left and right will depend on screen aspect
	HtCamera();
	HtCamera(const HtCamera& other)=delete;
	Vector2D m_screenCentre;			// Coordinates of the centre of the screen (width/2, height/2)
	Vector2D m_worldPosition;			// World coordinates for the centre of the screen	
	Rectangle2D m_cameraArea;			// The visible area of the screen in world coordinates
	double m_worldZoom;					// Scale factor for drawing. 
	double m_screenZoom;				// Base scale factor to make screen height 2000 units
	bool m_active;						// If true, will use world position and zoom. If not, just uses screen zoom and position

		// Returns the native coordinates from the given world coordinates
	// using the current camera position and zoom.
	Vector2D GetNativeFromWorldCoordinates(Vector2D worldPosition) const;

	// Returns the native coordinates from the given screen coordinates
	// using the current camera position and zoom.
	Vector2D GetNativeFromScreenCoordinates(Vector2D screenPosition) const;

	// Returns the world coordinates from the given native coordinates
	// using the current camera settings
	Vector2D GetWorldCoordinates(Vector2D nativeCoordinates) const;

	// Returns the screen coordinates from the given native coordinates
	// using the current camera settings
	Vector2D GetScreenCoordinates(Vector2D nativeCoordinates) const;

	// Returns the product of the size and the current scale. 
	// This effectively transforms a world scale to a native scale.
	double GetScaleFromWorld(double size) const;

	// Returns the product of the size and the current scale. 
	// This effectively transforms a screen scale to a native scale.
	double GetScaleFromScreen(double size) const;

	// Returns the size an object (i.e. length or radius) in the world
	// camera, based on the native size
	double GetWorldSize(double nativesize) const;

	// Returns the size an object (i.e. length or radius) in the screen
	// camera, based on the native size
	double GetScreenSize(double nativesize) const;

	// Returns a circle in native coordinates from the given world circle
	// using the current camera settings
	Circle2D GetCircleFromWorld(const Circle2D& circle) const;

	// Returns a circle in native coordinates from the given screen circle
	Circle2D GetCircleFromScreen(const Circle2D& circle) const;

	// Returns a rectangle in native coordinates from the given
	// rectangle in world coordinates
	// using the current camera settings
	Rectangle2D GetRectangleFromWorld(const Rectangle2D& rectangle) const;

	// Returns a rectangle in native coordinates from the given
	// rectangle in screen coordinates
	Rectangle2D GetRectangleFromScreen(const Rectangle2D& rectangle) const;

	// Returns a rectangle in native coordinates from the given
	// rectangle in world coordinates
	// using the current camera settings
	AngledRectangle2D GetAngledRectangleFromWorld(const AngledRectangle2D& rectangle) const;

	// Returns a rectangle in native coordinates from the given
	// rectangle in screen coordinates
	AngledRectangle2D GetAngledRectangleFromScreen(const AngledRectangle2D& rectangle) const;

	// Returns a segment in native coordinates from the given 
	// segment in world coordinates
	// using the current camera settings
	Segment2D GetSegmentFromWorld(const Segment2D& segment) const;

	// Returns a segment in native coordinates from the given 
	// segment in screen coordinates
	// using the current camera settings
	Segment2D GetSegmentFromScreen(const Segment2D& segment) const;

	// Calculates the current rectangle for the screen area and sets screen area to this
	void CalculateArea();
	
};

