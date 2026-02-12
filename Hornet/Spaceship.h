#pragma once
#include "GameObject.h"
class Spaceship :
    public GameObject
{
private:
    Vector2D m_velocity;
    Vector2D m_position;
    static const double ACCELERATION;

public:
    Spaceship();

    void Update(double frametime) override;

    void Initialise();

    void ProcessCollision(GameObject& other) override;
};

