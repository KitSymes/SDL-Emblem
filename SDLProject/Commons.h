#pragma once

struct Vector2D
{
	float x, y;

	Vector2D()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2D(float xPos, float yPos)
	{
		x = xPos;
		y = yPos;
	}
};