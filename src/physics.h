#pragma once
#include <iostream> // for debug

namespace physics
{
	// static point
	class Point {
	public:
		// position values
		float x = 0.0f, y = 0.0f;
		// constructors
		Point() {}
		Point(float x, float y) : x(x), y(y) {}
	};
	// scaler operations (overload addition and subtraction operators)
	Point operator+(Point& p, float f) { return Point(p.x + f, p.y + f); }
	Point operator-(Point& p, float f) { return Point(p.x - f, p.y - f); }
	// create alias 'Vector' for 'Point'
	typedef Point Vector;

	// static 1D object
	class Barrier1D {
	public:
		// object position and vertices in the virtual world
		Point p1, p2;
		float vertices[4] = { 0.0f };
		// constructor
		Barrier1D(float windowAspectRatio, int inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor((float)inverseScaleFactor), p1(p1), p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices()
		{
			vertices[0] = (float)(2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[1] = (float)(2 * (p1.y) / (inverseScaleFactor)) - 1;
			vertices[2] = (float)(2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[3] = (float)(2 * (p2.y) / (inverseScaleFactor)) - 1;
		}
	private:
		float windowAspectRatio;
		float inverseScaleFactor;
	};

	// static 2D object
	class Barrier2D {
	public:
		// object position, size, and vertices in the virtual world
		Point p1, p2;
		float vertices[8] = { 0.0f };
		// constructor
		Barrier2D(float windowAspectRatio, int inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor((float)inverseScaleFactor), p1(p1), p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices()
		{
			vertices[0] = (float)(2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[1] = (float)(2 * (p1.y) / (inverseScaleFactor)) - 1;
			vertices[2] = vertices[0];
			vertices[3] = (float)(2 * (p2.y) / (inverseScaleFactor)) - 1;
			vertices[4] = (float)(2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[5] = vertices[3];
			vertices[6] = vertices[4];
			vertices[7] = vertices[1];
		}
	private:
		float windowAspectRatio;
		float inverseScaleFactor;
	};
	
	// dynamic object
	class Object {
	public:
		// object position, size, velocity, and vertices in the virtual world
		Point p1, p2;
		Point prev_p1, prev_p2;
		Vector velocity;
		float vertices[8] = { 0.0f };
		// constructor
		Object(float windowAspectRatio, int inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor((float)inverseScaleFactor), p1(p1), p2(p2), prev_p1(p1), prev_p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices()
		{
			vertices[0] = (float)(2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[1] = (float)(2 * (p1.y) / (inverseScaleFactor)) - 1;
			vertices[2] = vertices[0];
			vertices[3] = (float)(2 * (p2.y) / (inverseScaleFactor)) - 1;
			vertices[4] = (float)(2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1;
			vertices[5] = vertices[3];
			vertices[6] = vertices[4];
			vertices[7] = vertices[1];
		}
		// calculate the object's next position and velocity after one timestep
		void calcTimeStep(float timeElapsed, float accelerationX = 0, float accelerationY = 0)
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
	private:
		float windowAspectRatio;
		float inverseScaleFactor;
	};

	// return true if a line intersects with another line
	bool lineIntersectsLine(Point& p1, Point& p2, Point& p3, Point& p4, Point& intersect)
	{
		return 0;
	}

	// return true if a point intersects with a 2D object/barrier defined by two points
	bool pointIntersectsObject(Point& p1, Point& p2, Point& p3)
	{
		if (p2.x > p3.x)
		{
			if (p1.x > p2.x || p1.x < p3.x)
				return false;
		}
		else if(p2.x < p3.x)
		{
			if (p1.x > p3.x || p1.x < p2.x)
				return false;
		}
		else
		{
			if (p1.x != p2.x)
				return false;
		}

		if (p2.y > p3.y)
		{
			if (p1.y > p2.y || p1.y < p3.y)
				return false;
		}
		else if (p2.y < p3.y)
		{
			if (p1.y > p3.y || p1.y < p2.y)
				return false;
		}
		else
		{
			if (p1.y != p2.y)
				return false;
		}

		return true;
	}
	// return true if a point intersects with a 2D barrier
	bool pointIntersectsObject(Point& p1, Barrier2D& bar)
	{
		return pointIntersectsObject(p1, bar.p1, bar.p2);
	}
	// return true if a point intersects with a 2D object
	bool pointIntersectsObject(Point& p1, Object& obj)
	{
		return pointIntersectsObject(p1, obj.p1, obj.p2);
	}

	// return true if a 2D object/barrier intersects with another 2D object/barrier
	bool objectIntersectsObject(Point& p1, Point& p2, Point& p3, Point& p4)
	{
		Point point(p1.x, p1.y);
		if (pointIntersectsObject(point, p3, p4))
			return true;
		point.x = p2.x;
		if (pointIntersectsObject(point, p3, p4))
			return true;
		point.x = p1.x;
		point.y = p2.y;
		if (pointIntersectsObject(point, p3, p4))
			return true;
		point.x = p2.x;
		if (pointIntersectsObject(point, p3, p4))
			return true;
		return false;
	}
	// return true if a 2D barrier intersects with a 2D object
	bool objectIntersectsBarrier(Object& obj, Barrier2D& bar)
	{
		return objectIntersectsObject(obj.p1, obj.p2, bar.p1, bar.p2);
	}
	// return true if a 2D object intersects with another 2D object
	bool objectIntersectsObject(Object& obj1, Object& obj2)
	{
		return objectIntersectsObject(obj1.p1, obj1.p2, obj2.p1, obj2.p2);
	}
};