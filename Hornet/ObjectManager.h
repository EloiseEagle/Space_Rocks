#pragma once

// 25/6/2025 Debug now shows frames per second not frames per minute
// 1/6/2025 Added more debug keys. HOME or TAB to toggle on and off. 
//			INSERT or BACKSPACE for slowdown.
//			PAGEUP/PAGEDOWN or EQUALS/MINUS to cycle targets
// Added key information to debug text
// Three stages of debug speed control: Normal, Slow, Frozen

// 16/9/25  Support for user to cycle between debug objects of same or different type using [ and ] keys.
// 16/9/25	Added SetDebugTarget()


#include "GameObject.h"
#include <list>
#include "gametimer.h"

class ObjectManager
{
private:
	std::list<GameObject*> m_allObjectList;
	std::list<GameObject*> m_colliderList;
	std::list<GameObject*> m_eventHandlerList;
	int m_currentScene;
	bool m_debugActive;
	int m_slowDownState;
	double m_frametime;
	GameObject* m_debugTarget;

	// Renders information about the current debug target
	void RenderDebugObject();

	// Sets up the object and sets the currentScene to zero
	// Also sets the gametimer
	ObjectManager();
	// Deletes all objects
	~ObjectManager();
	ObjectManager(const ObjectManager& other) = delete;
	ObjectManager(ObjectManager&& other) = delete;

public:

	static ObjectManager instance;

	// Returns a pointer to the closest active object of a given type to 
	// the specified location in the currentScene
	GameObject* FindClosestObject(Vector2D location, ObjectType ot);

	// Returns a pointer to the closest active object of a given type to 
// the specified location in the currentScene
	std::vector<GameObject*> GetAllObjectsOfType(ObjectType ot);

	// Adds a new item to the list of objects in the current scene.
	// Note: 
	// This function sets the GameObject's scene.
	// If you want to use a different scene, set it after using this function
	// Parameters:
	//  pNewItem - Pointer to the object to be added
	//  collides - If true, this object will also be
	//          added to the list of collidable objects
	void AddItem(GameObject* pNewItem);

	// Returns the number of objects currently in the Manager
	// Returns:
	//   The number of objects  in all scenes (including disabled objects 
	//    that have not been deleted)
	int GetNumObjects() const;

	// Deletes all objects in all scenes that have been set to "disabled".
	void DeleteInactiveItems();

	// Updates all objects in the current scene.
	void UpdateAll(double frametime);

	// Draws all objects in the current scene
	void RenderAll();

	// Checks for collisions between all objects in the current scene.
	// If any collide, ProcessCollision will be called for both objects
	void ProcessCollisions();

	// Deletes all objects in all scenes
	void DeleteAllObjects();			// Are you sure? (Y/N)

	// Sends the event to all objects that are set to receive events
	// Note: this is actually a fairly inefficient system at present,
	// but it is intended more for convenience than efficiency.
	// Note: Only active objects in the current scene will receive the event
	// Parameters:
	//    evt - the event to be sent to all objects
	void HandleEvent(Event evt);

	// Sets the current scene used by the object system
	// Only objects in the current scene will be drawn or updated.
	// Parameters:
	//  sceneNumber - the number of the scene which will become the current scene
	void SetCurrentScene(int sceneNumber);

	// Returns the number of the current scene.
	// Returns:
	//  the number of the current scene.
	int GetCurrentScene();

	// Sets ALL objects to inactive.
	void DeactivateAll();

	// Deactivates all objects with the corresponding type
	// Parameters:
	//    type - The type of object to be deleted
	void DeactivateType(ObjectType type);

	// Deactivates all objects with the specified scene
	// Parameters:
	//    sceneNumber - The number of the scene which objects 
	//            will be deleted from
	void DeactivateScene(int sceneNumber);

	// Renders debug information if debug display is active
	void RenderDebugDisplay();

	// Handles logic to change the current debug target using keypresses
	void CycleDebugObject();

	// Specifies the current debug target for display
	void SetDebugTarget(GameObject* pTarget);

	// Sets the debug display on or off)
	void SetDebugDisplay(bool debugOn);

	// Toggles the debug display between on and off
	void ToggleDebugDisplay();

};
