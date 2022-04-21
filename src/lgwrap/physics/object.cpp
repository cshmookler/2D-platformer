#include "object.h"

// Barrier1D: translate the object's position in the virtual world to coordinates on the window
void lgw::Barrier1D::setVertices(float xShift = 0.0f, float yShift = 0.0f)
{
	vertices[0] = (2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[1] = (2 * (p1.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[2] = (2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[3] = (2 * (p2.y) / inverseScaleFactor) - 1.0f + yShift;
}
// Barrier1D: constructor
lgw::Barrier1D::Barrier1D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
	: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2)
{
	setVertices();
}

// Barrier2D: translate the object's position in the virtual world to coordinates on the window
void lgw::Barrier2D::setVertices(float xShift = 0.0f, float yShift = 0.0f)
{
	vertices[0] = (2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[1] = (2 * (p1.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[2] = vertices[0];
	vertices[3] = (2 * (p2.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[4] = (2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[5] = vertices[3];
	vertices[6] = vertices[4];
	vertices[7] = vertices[1];
}
// Barrier2D: constructor
lgw::Barrier2D::Barrier2D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
	: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2)
{
	setVertices();
}

// Texture: translate the object's position in the virtual world to coordinates on the window
void lgw::Texture::setVertices(float xShift = 0.0f, float yShift = 0.0f)
{
	vertices[0] = (2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[1] = (2 * (p1.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[2] = vertices[0];
	vertices[3] = (2 * (p2.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[4] = (2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[5] = vertices[3];
	vertices[6] = vertices[4];
	vertices[7] = vertices[1];

	Point difference(vertices[4] - vertices[0], vertices[3] - vertices[1]);

	vertices[8] = 0.0f;
	vertices[9] = 0.0f;
	vertices[10] = 0.0f;
	vertices[11] = difference.y;
	vertices[12] = difference.x;
	vertices[13] = 0.0f;
	vertices[14] = difference.x;
	vertices[15] = difference.y;
}
// Texture: constructor
lgw::Texture::Texture(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
	: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2)
{
	setVertices();
}

// Object: translate the object's position in the virtual world to coordinates on the window
void lgw::Object::setVertices(float xShift = 0.0f, float yShift = 0.0f)
{
	vertices[0] = (2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[1] = (2 * (p1.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[2] = vertices[0];
	vertices[3] = (2 * (p2.y) / inverseScaleFactor) - 1.0f + yShift;
	vertices[4] = (2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
	vertices[5] = vertices[3];
	vertices[6] = vertices[4];
	vertices[7] = vertices[1];
}
// Object: constructor
lgw::Object::Object(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
	: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2), prev_p1(p1), prev_p2(p2)
{
	setVertices();
}
// Object: calculate the object's next position and velocity after one timestep
void lgw::Object::calcTimeStep(float timeElapsed, float accelerationX = 0.0f, float accelerationY = 0.0f)
{
	prev_p1.x = p1.x;
	prev_p1.y = p1.y;
	prev_p2.x = p2.x;
	prev_p2.y = p2.y;
	float xShift = (velocity.x * timeElapsed) + ((accelerationX * (timeElapsed * timeElapsed)) / 2);
	float yShift = (velocity.y * timeElapsed) + ((accelerationY * (timeElapsed * timeElapsed)) / 2);
	p1.x += xShift;
	p2.x += xShift;
	p1.y += yShift;
	p2.y += yShift;
	velocity.x += accelerationX * timeElapsed;
	velocity.y += accelerationY * timeElapsed;
}