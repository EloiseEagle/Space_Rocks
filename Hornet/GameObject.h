#pragma once

#include "objecttypes.h"
#include "vector2D.h"
#include "Shapes.h"
#include "HtGraphics.h"
#include "event.h" 
#include <vector>

// 25/5/25 Update no longer crashes if there are no images
// 1/6/25 In debug, now displays a number for the TYPE of object


// Base class for all game objects
class GameObject
{
private:
	bool m_active;				// Indicates if the object is still in use. If set to false, the object will be deleted by the objectManager.
	int m_sceneNumber;			// The number of the scene that this GameObject belongs to
	bool m_locked;				// Sets to true after added to object manager. If true, cannot
								// change draw depth, collidable or handle events.
	int m_drawDepth;			// default is 0. Objects with lower numbers will be drawn before objects with higher numbers. (And will therefore be
								// "background".) Cannot change after the GameObject is locked.
	bool m_handleEvents;		// Whether or not to process events. Default is false. Cannot change after the GameObject is locked.
	bool m_collidable;			// Whether or not the object should collide. Default is false. Cannot change after the GameObject is locked.
	int m_debugLineNumber;		// Used to draw the debug information on the next line.
	Rectangle2D m_defaultCollisionShape;
protected:
	std::vector<PictureIndex> m_images;		// Indices of the pictures loaded using LoadPicture. If a picture is not loaded, this will be 0
	Vector2D m_position;		// Position of the object
	double m_scale;			// Scale of the loaded image. If the scale is <1, the obect will be drawn smaller than the image
	double m_angle;				// Angle of rotation of the image. 0 is upright. Angle in radians clockwise
	int m_imageNumber;			// Index of the current image in m_images;
	double m_transparency;
	// Call these function in RenderDebug to display information when debugging
	void AddDebugLine(const char* title, int value);
	void AddDebugLine(const char* title, double value);
	void AddDebugLine(const char* title, const char* value);
	void AddDebugLine(const char* title, bool value);
	void AddDebugLine(const char* title, Vector2D value);
public:
	const ObjectType TYPE;		// The type of the object. Must be set to one of the values in objecttypes.h

	// Constructor. All subclasses must set a type using Subclass::Subclass(): GameObject(TYPE)
	explicit GameObject(ObjectType type);
	GameObject();

	// Destructor
	virtual ~GameObject();

	// Returns the type of the object
	ObjectType GetType() const;

	// Sets m_active to false. The object will be deleted by the engine
	void Deactivate();

	// Returns whether the object is active or not
	inline bool IsActive() const
	{
		return m_active;
	}

	// Returns the position of the object
	Vector2D GetPosition() const;

	// Load an image and stores the index in m_image. 
	// If the filename is invalid, m_image will be set to 0
	void LoadImage(const char* filename);

	// Returns true if the object has collided with the other object
	bool HasCollided(GameObject& other);

	virtual void Initialise(const char* imagefile, Vector2D position, double angle=0, double scale=1.0);

	// Draws the object using the MyDrawEngine. The position is used for the centre of the object.
	// The position and scale will be transformed by the draw engine's camera
	virtual void Render();

	// Used to handle the result of any collision
	// Default behaviour is to do nothing
	virtual void ProcessCollision(GameObject& other);

	// Used to process each frame
	// Default behaviour is to do nothing, but it
	// will set the position of the default collision rectangle if collision
	// is enabled.
	virtual void Update(double frametime);

	// Returns the collision shape (Rectangle2D, Circle2D, Segment2D or Point2D)
	// The shape that it returns must be a member variable, not a local variable
	// You should override this, but the default is to provide a Rectangle2D
	// that matches the dimensions of the loaded image (scaled by m_imageScale).
	virtual IShape2D& GetCollisionShape();

	// Default behaviour is to do nothing
	virtual void HandleEvent(Event evt);

	// Displays debug information about the object
	// You can override this to add new information, but call GameObject::RenderDebug first.
	virtual void RenderDebug();

	// Displays the current debug shape on the screen
	// Called from RenderDebug.
	void RenderDebugShape();

	// Returns the number of the scene that this GameObject belongs to
	int GetSceneNumber() const;

	// Sets the scene number of the object. Only objects in the current scene will be drawn, updated or collide.
	void SetSceneNumber(int sceneNumber);

	// Lockable accessors . These values should not changed after the object locked
	// when adding to the ObjectManager.
	
	// Returns the depth of drawing. Lower numbers are drawn first.
	int GetDrawDepth() const;

	double GetScale() const;

	// Returns true if the objects can handle events.
	bool CanHandleEvents() const;

	// Returns true if collidable
	bool IsCollidable();

	// Sets the draw depth of the object. Small numbers are "background". larger numbers
	// are "foreground". You should set this before adding the object to the object
	// manager. You can only set this once. If you try to change it later, this
	// function will ignore the instruction.
	// The default draw depth is 0.
	void SetDrawDepth(int depth);

	// Sets the object to collidable. This will have no effect after the Object is Lock()ed.
	void SetCollidable();

	// Sets the object to handle events. This will have no effect after the Object is Lock()ed.
	void SetHandleEvents();

	// After this is called, no changes can be made to collidable, draw depth or handle events
	void Lock();
};

