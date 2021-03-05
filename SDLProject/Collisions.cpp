#include "Collisions.h"
#include "MarioCharacter.h"

Collisions* Collisions::m_instance = nullptr;

Collisions::Collisions()
{

}

Collisions::~Collisions()
{
	m_instance = nullptr;
}

Collisions* Collisions::Instance()
{
	if (!m_instance)
	{
		m_instance = new Collisions;
	}

	return m_instance;
}

bool Collisions::Circle(Vector2D pos1, double radius1, Vector2D pos2, double radius2)
{
	Vector2D vec = Vector2D((pos1.x - pos2.x), (pos1.y - pos2.y));
	double distance = sqrt((vec.x * vec.x) + (vec.y * vec.y));
	double combined_distance = (radius1 + radius2);
	return distance < combined_distance;
}

bool Collisions::Box(Rect2D rect1, Rect2D rect2)
{
	/*if (rect1.x + (rect1.width / 2) > rect2.x &&
		rect1.x + (rect1.width / 2) < rect2.x + rect2.width &&
		rect1.y + (rect1.height / 2) > rect2.y &&
		rect1.y + (rect1.height / 2) < rect2.y + rect2.height)
		return true;
	return false;*/

	// Collision tests
	if (rect1.x + rect1.width < rect2.x || rect1.x > rect2.x + rect2.width) return false;
	if (rect1.y + rect1.height < rect2.y || rect1.y > rect2.y + rect2.height) return false;

	return true;
}
