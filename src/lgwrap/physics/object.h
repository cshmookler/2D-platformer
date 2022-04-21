#pragma once

#include <iostream>
#include <cmath>

namespace lgw {
	// static point
	class Point {
	public:
		// position values
		float x = 0.0f, y = 0.0f;
		// constructors
		inline Point() {}
		inline Point(float x, float y) : x(x), y(y) {}
	};
	// scaler operations (overload addition and subtraction operators)
	inline Point operator+(Point& p, float f) { return Point(p.x + f, p.y + f); }
	inline Point operator-(Point& p, float f) { return Point(p.x - f, p.y - f); }
	// create alias 'Vector' for 'Point'
	typedef Point Vector;

	// static 1D object
	class Barrier1D {
	public:
		// object position and vertices in the virtual world
		Point p1, p2;
		float vertices[4] = { 0.0f };
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift, float yShift);
		// constructor
		Barrier1D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2);
	private:
		float& windowAspectRatio;
		float& inverseScaleFactor;
	};

	// static 2D object
	class Barrier2D {
	public:
		// object position, size, and vertices in the virtual world
		Point p1, p2;
		float vertices[8] = { 0.0f };
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift, float yShift);
		// constructor
		Barrier2D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2);
	private:
		float& windowAspectRatio;
		float& inverseScaleFactor;
	};

	// texture object (static 2D object that has a texture)
	class Texture {
	public:
		// object position, size, and vertices in the virtual world
		Point p1, p2;
		float vertices[16] = { 0.0f };
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift, float yShift);
		// constructor
		Texture(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2);
	private:
		float& windowAspectRatio;
		float& inverseScaleFactor;
	};

	// dynamic object
	class Object {
	public:
		// object position, size, velocity, and vertices in the virtual world
		Point p1, p2;
		Point prev_p1, prev_p2;
		Vector velocity;
		float vertices[8] = { 0.0f };
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift, float yShift);
		// constructor
		Object(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2);
		// calculate the object's next position and velocity after one timestep
		void calcTimeStep(float timeElapsed, float accelerationX, float accelerationY);
	private:
		float& windowAspectRatio;
		float& inverseScaleFactor;
	};
}