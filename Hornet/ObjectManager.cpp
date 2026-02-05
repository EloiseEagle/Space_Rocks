#include "HtKeyboard.h"
#include "ObjectManager.h"
#include <algorithm>
#include "HtCamera.h"

ObjectManager ObjectManager::instance;

ObjectManager::ObjectManager()
{
	m_debugTarget = nullptr;
	m_currentScene = 0;
	m_slowDownState = 0;
	m_debugActive = false;
	m_frametime = 0;
}

ObjectManager::~ObjectManager()
{
	DeleteAllObjects();
}

void ObjectManager::AddItem(GameObject* pNewItem)
{
	if (pNewItem)
	{
		// Can't change certain things any longer, such as draw depth
		pNewItem->Lock();

		// Use current scene
		pNewItem->SetSceneNumber(m_currentScene);

		// Find position of first item with a higher or equal Draw Depth

		auto it = m_allObjectList.begin();

		while (it != m_allObjectList.end() && (*it)->GetDrawDepth() < pNewItem->GetDrawDepth())
		{
			++it;
		}
		m_allObjectList.insert(it, pNewItem);

		if (pNewItem->IsCollidable())
		{
			m_colliderList.push_back(pNewItem);
		}
		if (pNewItem->CanHandleEvents())
		{
			m_eventHandlerList.push_back(pNewItem);
		}
	}
}

// Returns a pointer to the closest active object of a given type to 
// the specified location in the currentScene
GameObject* ObjectManager::FindClosestObject(Vector2D location, ObjectType ot)
{
	GameObject* pClosest = nullptr;
	double distance2 = 9e9f;	// A very long way away.
	for (GameObject* pNext : m_allObjectList)
	{
		if (pNext &&
			pNext->GetType() == ot &&           // Is the correct type of object
			pNext->GetSceneNumber() == m_currentScene &&  // Is in the current active scene
			pNext->IsActive() &&                          // Is active
			(pNext->GetPosition() - location).magnitudeSquared() < distance2)     // Is closer than the previous closest
		{
			distance2 = (pNext->GetPosition() - location).magnitudeSquared();
			pClosest = pNext;
		}
	}
	return pClosest;
}

std::vector<GameObject*> ObjectManager::GetAllObjectsOfType(ObjectType ot)
{
	std::vector<GameObject*> answer;
	for (GameObject* pNext : m_allObjectList)
	{
		if (pNext &&
			pNext->GetType() == ot &&           // Is the correct type of object
			pNext->GetSceneNumber() == m_currentScene)  // Is in the current active scene
		{
			answer.push_back(pNext);
		}
	}
	return answer;
}

void ObjectManager::DeleteInactiveItems()
{

	// Remove all inactive objects from collider list
	auto itc = std::remove_if(m_colliderList.begin(), m_colliderList.end(), [](GameObject* pGO) { return !pGO->IsActive(); });
	m_colliderList.erase(itc, m_colliderList.end());

	// Remove all inactive objects from event handler list
	auto ite = std::remove_if(m_eventHandlerList.begin(), m_eventHandlerList.end(), [](GameObject* pGO) { return !pGO->IsActive(); });
	m_colliderList.erase(ite, m_eventHandlerList.end());

	// Delete all inactive objects
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		if (!(*it)->IsActive())
		{
#ifdef _DEBUG
			if(*it == m_debugTarget)
				m_debugTarget = nullptr;
#endif // DEBUG
			delete* it;
			*it = nullptr;
		}
	}

	// Remove all inactive objects from master list
	auto ita = std::remove(m_allObjectList.begin(), m_allObjectList.end(), nullptr);
	m_allObjectList.erase(ita, m_allObjectList.end());
}

void ObjectManager::UpdateAll(double frametime)
{

#ifdef _DEBUG
	if (m_debugActive)
	{
		if (m_slowDownState == 1)
			frametime /= 5;
		if (m_slowDownState == 2)
			frametime /= 50;
		else if (m_slowDownState == 3)
			frametime = 0;
	}
#endif // _DEBUG

	m_frametime = frametime;

	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		if ((*it)->IsActive() && (*it)->GetSceneNumber() == m_currentScene)
		{
			(*it)->Update(float(frametime));
		}
	}
}

void ObjectManager::RenderAll()
{
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		if ((*it)->IsActive() && (*it)->GetSceneNumber() == m_currentScene)
		{
			(*it)->Render();
		}
	}

}

void ObjectManager::ProcessCollisions()
{
	//  collisions
	auto it1 = m_colliderList.begin();

	for (; it1 != m_colliderList.end(); ++it1)
	{
		auto it2 = it1;
		it2++;

		for (; it2 != m_colliderList.end(); ++it2)
		{
			if ((*it1)->IsActive() && (*it2)->IsActive()
				&& (*it1)->GetSceneNumber() == m_currentScene
				&& (*it2)->GetSceneNumber() == m_currentScene)
			{
				if ((*it1)->HasCollided(**it2))
				{
					(*it1)->ProcessCollision(**it2);
					(*it2)->ProcessCollision(**it1);
				}
			}
		}
	}
}


int ObjectManager::GetNumObjects() const
{
	return (int)m_allObjectList.size();
}

void ObjectManager::DeleteAllObjects()
{
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		delete* it;
	}

	m_allObjectList.clear();
	m_colliderList.clear();
	m_eventHandlerList.clear();
#ifdef _DEBUG
	m_debugTarget = nullptr;
#endif // DEBUG


}

void ObjectManager::HandleEvent(Event evt)
{
	auto it = m_eventHandlerList.begin();

	for (; it != m_eventHandlerList.end(); ++it)
	{
		(*it)->HandleEvent(evt);
	}
}

// Sets the current scene used by the object system
// Only objects in the current scene will be drawn or updated.
void ObjectManager::SetCurrentScene(int sceneNumber)
{
	m_currentScene = sceneNumber;
}

// Returns the number of the current scene.
int ObjectManager::GetCurrentScene()
{
	return m_currentScene;
}

// Sets ALL objects to inactive.
void ObjectManager::DeactivateAll()
{
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		(*it)->Deactivate();
	}
}

// Deactivates all objects with the corresponding type
void ObjectManager::DeactivateType(ObjectType type)
{
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		if ((*it) && (*it)->GetSceneNumber() == m_currentScene &&
			(*it)->GetType() == type)
			(*it)->Deactivate();
	}
}

// Deactivates all objects with the specified scene
void ObjectManager::DeactivateScene(int sceneNumber)
{
	auto it = m_allObjectList.begin();

	for (; it != m_allObjectList.end(); ++it)
	{
		if ((*it)->GetSceneNumber() == sceneNumber)
			(*it)->Deactivate();
	}
}


void ObjectManager::SetDebugDisplay(bool debugOn)
{
	m_debugActive = debugOn;
}

void ObjectManager::ToggleDebugDisplay()
{
	SetDebugDisplay(!m_debugActive);
}

void ObjectManager::CycleDebugObject()
{
	if (!m_debugActive&& (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_HOME)|| HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_TAB)))
	{
		SetDebugDisplay(true);
		if (m_allObjectList.size() > 0)
			m_debugTarget = *m_allObjectList.begin();
		else
			m_debugTarget = nullptr;
	}
	if (m_debugActive)
	{
		// Turning debug display off
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_HOME) || HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_TAB))
		{
			SetDebugDisplay(false);
			m_debugTarget = nullptr;
		}

		// Activates slow time
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_INSERT) || HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_BACKSPACE))
		{
			++m_slowDownState;
			if (m_slowDownState > 3) m_slowDownState = 0;
		}

		// Cycles to next object
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_PAGEUP) || HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_EQUALS))
		{
			if (m_allObjectList.size() > 0)
			{
				auto it = std::find(m_allObjectList.begin(), m_allObjectList.end(), m_debugTarget);
				if (it == m_allObjectList.end())
				{
					it = m_allObjectList.begin();
					m_debugTarget = *it;
				}
				else
				{
					++it;
					if (it != m_allObjectList.end())
						m_debugTarget = *it;
					else
					{
						m_debugTarget = *m_allObjectList.begin();
					}
				}
			}
			else
			{
				m_debugTarget = nullptr;
			}
		}

		// Cycles to previous object
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_PAGEDOWN) || HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_MINUS))
		{
			if (m_allObjectList.size() > 0)
			{
				auto it = std::find(m_allObjectList.begin(), m_allObjectList.end(), m_debugTarget);
				if (it == m_allObjectList.begin()) // If current target is first, cyle to end
				{
					it = m_allObjectList.end();
				}
				it--;
				m_debugTarget = *it;
			}
			else
			{
				m_debugTarget = nullptr;
			}
		}

		// Cycles to next object of same type
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_LEFTBRACKET))
		{
			if (m_allObjectList.size() > 0)
			{
				// Get an iterator to the current target
				auto it = std::find(m_allObjectList.begin(), m_allObjectList.end(), m_debugTarget);
				if (it == m_allObjectList.end())			// If couldn't find it
				{
					it = m_allObjectList.begin();			// Just use the start of the list
					m_debugTarget = *it;
				}
				else
				{
					++it;									// Go to the next one
					while (*it != m_debugTarget)			// Repeat until you return to the current target
					{
						if (it == m_allObjectList.end())	// Wrap around if you reach the end
							it = m_allObjectList.begin();	
						if ((*it)->GetType() == m_debugTarget->GetType())	// If the next one has the same type
						{
							m_debugTarget = *it;			// Use it
							break;							// Stop looking
						}
						++it;								// Next one
					}
				}
			}
			else					// If there are no objects in the list
			{
				m_debugTarget = nullptr;
			}
		}


		// Cycles to next object of different type
		if (HtKeyboard::instance.NewKeyPressed(SDL_SCANCODE_RIGHTBRACKET))    
		{
			if (m_allObjectList.size() > 0)
			{
				// Get an iterator to the current target
				auto it = std::find(m_allObjectList.begin(), m_allObjectList.end(), m_debugTarget);
				if (it == m_allObjectList.end())			// If couldn't find it
				{
					it = m_allObjectList.begin();			// Just use the start of the list
					m_debugTarget = *it;
				}
				else
				{
					++it;									// Go to the next one
					while (*it != m_debugTarget)			// Repeat until you return to the current target
					{
						if (it == m_allObjectList.end())	// Wrap around if you reach the end
							it = m_allObjectList.begin();
						if (m_debugTarget && (*it)->GetType() != m_debugTarget->GetType())	// If the next one has a different type
						{
							m_debugTarget = *it;			// Use it
							break;							// Stop looking
						}
						++it;								// Next one
					}
				}
			}
			else					// If there are no objects in the list
			{
				m_debugTarget = nullptr;
			}
		}
	}
}

void ObjectManager::SetDebugTarget(GameObject* pTarget)
{
	m_debugTarget = pTarget;
}

void ObjectManager::RenderDebugDisplay()
{
	if (m_debugActive)
	{
		bool cameraPreviouslyActive = HtCamera::instance.IsCameraActive();
		HtCamera::instance.UseCamera(false);

		// Write general information
		HtGraphics::instance.WriteTextAligned(-1400, 990, "Keys: INSERT/BACKSPACE, PAGEUP/MINUS, PAGEDOWN/EQUALS, HOME/TAB, [, ] ", HtGraphics::WHITE, 2);
		// Number of objects
		HtGraphics::instance.WriteTextAligned(-1400, 950, "Num objects: ", HtGraphics::LIGHTGREEN, 2);
		HtGraphics::instance.WriteIntAligned(-800, 950, (int)m_allObjectList.size(), HtGraphics::LIGHTGREEN, 2);

		// Frame time
		HtGraphics::instance.WriteTextAligned(-1400, 910, "Frame Time: ", HtGraphics::LIGHTGREEN, 2);
		HtGraphics::instance.WriteFloatAligned(-800, 910, m_frametime, HtGraphics::LIGHTGREEN, 2);
		// Frame rate
		HtGraphics::instance.WriteTextAligned(-1400, 870, "Frame Rate: ", HtGraphics::LIGHTGREEN, 2);
		HtGraphics::instance.WriteFloatAligned(-800, 870, 1 / m_frametime, HtGraphics::LIGHTGREEN, 2);
		if (m_slowDownState==1)
			HtGraphics::instance.WriteTextAligned(-1400, 840, "Slowdown engaged", HtGraphics::RED, 2);
		else if (m_slowDownState == 2)
			HtGraphics::instance.WriteTextAligned(-1400, 840, "Super slowdown engaged", HtGraphics::RED, 2);
		else if (m_slowDownState == 3)
			HtGraphics::instance.WriteTextAligned(-1400, 840, "Game frozen", HtGraphics::RED, 2);
		

		// Render the debug target
		RenderDebugObject();

		HtCamera::instance.UseCamera(cameraPreviouslyActive);

		if (m_debugTarget)
		{
			m_debugTarget->RenderDebugShape();
		}
	}
}

void ObjectManager::RenderDebugObject()
{
	// Display information for the current target
	if (m_debugActive && m_debugTarget)
	{
		m_debugTarget->RenderDebug();
	}
}

