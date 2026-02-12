#pragma once
#include "GameObject.h"
class Rock :
    public GameObject
{
private:
    Vector2D m_velocity;
    Circle2D m_collisionShape;
public:
    Rock();

    void Update(double frametime) override;

    void ProcessCollision(GameObject& other) override;

    void Initialise();

    IShape2D& GetCollisionShape() override;
};

