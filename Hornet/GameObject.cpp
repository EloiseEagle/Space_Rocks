#include "GameObject.h"
#include "HtGraphics.h"
#include "ErrorLogger.h"

const int DEBUGLINESEPARATION = 30;
const double DEBUGTEXTSCALE = 0.75;
const int DEBUGTEXTFONT = 2;
const int DEBUGTOPOFTEXT = 760;

void GameObject::AddDebugLine(const char* title, int value)
{
	HtGraphics::instance.WriteTextAligned(-1400, DEBUGTOPOFTEXT - DEBUGLINESEPARATION *m_debugLineNumber, title, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	HtGraphics::instance.WriteIntAligned( - 800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, value, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	m_debugLineNumber++;
}

void GameObject::AddDebugLine(const char* title, double value)
{
	HtGraphics::instance.WriteTextAligned(-1400, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, title, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	HtGraphics::instance.WriteFloatAligned(-800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, value, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	m_debugLineNumber++;
}

void GameObject::AddDebugLine(const char* title, Vector2D value)
{
	HtGraphics::instance.WriteTextAligned(-1400, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, title, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	std::string vec = "( " + std::to_string(value.XValue) + " , "
		+ std::to_string(value.YValue) + " )";
	HtGraphics::instance.WriteTextAligned(-800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, vec, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	m_debugLineNumber++;
}

void GameObject::AddDebugLine(const char* title, const char* value)
{
	HtGraphics::instance.WriteTextAligned(-1400, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, title, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	HtGraphics::instance.WriteTextAligned(-800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, value, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	m_debugLineNumber++;
}

void GameObject::AddDebugLine(const char* title, bool value)
{
	HtGraphics::instance.WriteTextAligned(-1400, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, title, HtGraphics::LIGHTGREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	if(value)
		HtGraphics::instance.WriteTextAligned(-800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, "true", HtGraphics::GREEN, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	else
		HtGraphics::instance.WriteTextAligned(-800, DEBUGTOPOFTEXT - DEBUGLINESEPARATION * m_debugLineNumber, "false", HtGraphics::RED, DEBUGTEXTFONT, DEBUGTEXTSCALE);
	m_debugLineNumber++;
}

GameObject::GameObject(ObjectType type) : TYPE(type)
{
	m_active = true;
	m_imageNumber = -1;
	m_scale = 1.0;
	m_angle = 0;
	m_handleEvents = false;
	m_drawDepth = 0;
	m_sceneNumber = 0;
	m_collidable = false;
	m_handleEvents = false;
	m_debugLineNumber = 0;
	m_locked = false;
	m_transparency = 0;
}

GameObject::GameObject(): GameObject(ObjectType::UNKNOWN)
{
}

// Returns the position of the object
Vector2D GameObject::GetPosition() const
{
	return m_position;
}

void GameObject::SetDrawDepth(int depth)
{
	if (!m_locked)
		m_drawDepth = depth;
#ifdef _DEBUG
	else
		ErrorLogger::Write("You have called SetDrawDepth() on an object after it has been locked.");
#endif // DEBUG
}

void GameObject::SetCollidable()
{
	if (!m_locked)
		m_collidable = true;
#ifdef _DEBUG
	else
		ErrorLogger::Write("You have called SetCollidable() on an object after it has been locked.");
#endif // DEBUG
}

void GameObject::SetHandleEvents()
{
	if (!m_locked)
		m_handleEvents = true;
#ifdef _DEBUG
	else
		ErrorLogger::Write("You have called SetHandleEvents() on an object after it has been locked.");
#endif // DEBUG
}

void GameObject::Lock()
{
	m_locked = true;
}

void GameObject::RenderDebug()
{
	m_debugLineNumber = 0;
	AddDebugLine("Active:", m_active);
	AddDebugLine("Locked:", m_locked);
	AddDebugLine("Scene:", m_sceneNumber);
	AddDebugLine("Draw depth:", m_drawDepth);
	AddDebugLine("Events:", m_handleEvents);
	AddDebugLine("Collidable:", m_collidable);

	AddDebugLine("Num images:", (int)m_images.size());
	for (PictureIndex next : m_images)
	{
		if (m_images[m_imageNumber] == next)
		{
			if (next == NO_PICTURE_INDEX)
			{
				AddDebugLine("Image--->", false);
			}
			else
			{
				AddDebugLine("Image--->", (int)next);
			}

		}
		else
		{
			if (next == NO_PICTURE_INDEX)
			{
				AddDebugLine("Image:", false);
			}
			else
			{
				AddDebugLine("Image:", (int)next);
			}
		}
	}

	AddDebugLine("Image in use:", m_imageNumber);
	AddDebugLine("Scale:", m_scale);
	AddDebugLine("Transparency:", m_transparency);
	AddDebugLine("Angle:", m_angle);
	AddDebugLine("Position:", m_position);
	AddDebugLine("Type:", (int)TYPE);
}

bool GameObject::CanHandleEvents() const
{
	return m_handleEvents;
}

int GameObject::GetDrawDepth() const
{
	return m_drawDepth;
}

double GameObject::GetScale() const
{
	return m_scale;
}

GameObject::~GameObject()
{

}

IShape2D& GameObject::GetCollisionShape()
{
	// Default behaviour is to return the default collision rectangle.
	return m_defaultCollisionShape;
}

void GameObject::HandleEvent(Event evt)
{
	// Default behaviour is to do nothing
}

ObjectType GameObject::GetType() const
{
	return TYPE;
}

void GameObject::Deactivate()
{
	m_active = false;

}

bool GameObject::IsCollidable()
{
	return m_collidable;
}

void GameObject::LoadImage(const char* filename)
{
	m_images.push_back(HtGraphics::instance.LoadPicture(filename));
	
	if (m_imageNumber < 0)
		m_imageNumber = 0;
}

bool GameObject::HasCollided(GameObject& otherObject)
{
	return(m_active && otherObject.m_active && GetCollisionShape().Intersects(otherObject.GetCollisionShape()));
}

void GameObject::Initialise(const char* imagefile, Vector2D position, double angle, double scale)
{
	m_active = true;
	m_angle = angle;
	m_collidable = false;
	m_drawDepth = 0;
	m_handleEvents = false;
	m_imageNumber = 0;
	LoadImage(imagefile);
	m_scale = scale;
	m_position = position;
	m_sceneNumber = 0;


}



void GameObject::Render()
{
	if (m_imageNumber >= 0 && m_imageNumber<int(m_images.size()))
	{
		HtGraphics::instance.DrawAt(m_position, m_images[m_imageNumber], m_scale, m_angle, m_transparency);
	}
	else
	{
		HtGraphics::instance.WriteTextCentered(m_position, "No image", HtGraphics::instance.LIGHTGREEN, 0);
	}
}

void GameObject::ProcessCollision(GameObject& other)
{
	// Default behaviour is to do nothing.
}

void GameObject::Update(double frametime)
{
	// Default is to not move, but set the height and width of the default rectangle
	// if collision is active
	if (m_collidable)
	{
		int width = 0;
		int height = 0;
		if (m_images.size()>0 && (int)m_images[m_imageNumber] >= 0)
		{
			width = int(HtGraphics::instance.GetWidthOfPicture(m_images[m_imageNumber])* m_scale);
			height = int(HtGraphics::instance.GetHeightOfPicture(m_images[m_imageNumber]) * m_scale);
		}
		m_defaultCollisionShape.PlaceAt(
				m_position + Vector2D(-width / 2, -height / 2),
				m_position + Vector2D(width / 2, height / 2)   );
	}
}

void GameObject::RenderDebugShape()
{
	IShape2D& shape = GetCollisionShape();

	static const Colour DEBUGBROWN(200, 250, 75, 0);	// Transparent brown
	static const Colour DEBUGGREEN(125, 0, 255, 0);	// Transparent green

	if (!m_collidable)
	{
		Circle2D dot(m_position, 20);

		HtGraphics::instance.FillCircle(dot, DEBUGBROWN);
	}
	else if (typeid(shape) == typeid(Rectangle2D))
	{
		HtGraphics::instance.FillRect(*reinterpret_cast<Rectangle2D*>( & shape), DEBUGGREEN);
	}
	else if (typeid(shape) == typeid(Circle2D))
	{
		HtGraphics::instance.FillCircle(*reinterpret_cast<Circle2D*>(&shape), DEBUGGREEN);
	}
	else if (typeid(shape) == typeid(Segment2D))
	{
		HtGraphics::instance.DrawSegment(*reinterpret_cast<Segment2D*>(&shape), DEBUGGREEN);
	}

	else if (typeid(shape) == typeid(AngledRectangle2D))
	{
		HtGraphics::instance.FillAngledRect(*reinterpret_cast<AngledRectangle2D*>(&shape), DEBUGGREEN);
	}
	else // Point2D or something undefined
	{
		Segment2D vertical;
		vertical.PlaceAt(m_position + Vector2D(0, 20), m_position + Vector2D(0, -20));
		Segment2D horizontal;
		horizontal.PlaceAt(m_position + Vector2D(0, 20), m_position + Vector2D(0, -20));

		HtGraphics::instance.DrawSegment(vertical, DEBUGBROWN);
		HtGraphics::instance.DrawSegment(horizontal, DEBUGBROWN);
	}
}

// Returns the number of the scene that this GameObject belongs to
int GameObject::GetSceneNumber() const
{
	return m_sceneNumber;
}

// Sets the scene number of the object. Only objects in the current scene will be drawn or updated.
void GameObject::SetSceneNumber(int sceneNumber)
{
	m_sceneNumber = sceneNumber;
}



