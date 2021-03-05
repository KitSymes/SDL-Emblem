#pragma once
#ifndef _COLLISIONS_H
#define _COLLISIONS_H
#include "Commons.h"
class MarioPlayer;
class Collisions
{
private:
	Collisions();
	static Collisions* m_instance;
public:
	~Collisions();
	static Collisions* Instance();

	bool Circle(Vector2D pos1, double radius1, Vector2D pos2, double radius2);
	bool Box(Rect2D rect1, Rect2D rect2);
};
#endif
