#include "Spaceship.h"
#include "HtKeyboard.h"

const double Spaceship::ACCELERATION = 300;

Spaceship::Spaceship() : GameObject(ObjectType::SPACESHIP)
{

}

void Spaceship::Initialise()
{
	m_position.set(0, 0);
	m_velocity.set(0, 0);
	LoadImage("assets/ship.bmp");
	m_scale = 2;
}

void Spaceship::ProcessCollision(GameObject& other)
{

}

void Spaceship::Update(double frametime)
{
	if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_A))
	{
		m_angle = m_angle - 90 * frametime;
	}
	if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_D))
	{
		m_angle = m_angle + 90 * frametime;
	}
	if (HtKeyboard::instance.KeyPressed(SDL_SCANCODE_W))
	{
		Vector2D acceleration;
		acceleration.setBearing(m_angle, ACCELERATION);
		m_velocity = m_velocity + acceleration * frametime;
		
	}
	m_position = m_position + m_velocity * frametime;
}