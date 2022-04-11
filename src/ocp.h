#pragma once

#include <iostream> // for debug
#include <cmath> // for advanced math functions

namespace ocp // objects, collision, and physics
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
		Barrier1D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift = 0.0f, float yShift = 0.0f)
		{
			vertices[0] = (2 * (p1.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
			vertices[1] = (2 * (p1.y) / inverseScaleFactor) - 1.0f + yShift;
			vertices[2] = (2 * (p2.x) / (windowAspectRatio * inverseScaleFactor)) - 1.0f + xShift;
			vertices[3] = (2 * (p2.y) / inverseScaleFactor) - 1.0f + yShift;
		}
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
		// constructor
		Barrier2D(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift = 0.0f, float yShift = 0.0f)
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
		// constructor
		Object(float& windowAspectRatio, float& inverseScaleFactor, Point p1, Point p2)
			: windowAspectRatio(windowAspectRatio), inverseScaleFactor(inverseScaleFactor), p1(p1), p2(p2), prev_p1(p1), prev_p2(p2)
		{
			setVertices();
		}
		// translate the object's position in the virtual world to coordinates on the window
		void setVertices(float xShift = 0.0f, float yShift = 0.0f)
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
		float& windowAspectRatio;
		float& inverseScaleFactor;
	};

	// eliminate small positional errors between the first position (p1) and the second position (p2)
	void voidMinorPosDiff(Point& p1, Point& p2, float& errorMargin)
	{
		if (std::abs(p1.x - p2.x) <= errorMargin)
			p1.x = p2.x;
		if (std::abs(p1.y - p2.y) <= errorMargin)
			p1.y = p2.y;
	}

	// return true if a point and a line segment on the ame axis intersect
	bool pointIntersectsLineOnSameAxis(float a, float b1, float b2)
	{
		if (b1 > b2)
		{
			if (a >= b2 && a <= b1)
				return true;
		}
		else
		{
			if (a >= b1 && a <= b2)
				return true;
		}
		return false;
	}

	// return true if two line segments on the same axis intersect
	bool lineIntersectsLineOnSameAxis(float a1, float a2, float b1, float b2)
	{
		return (pointIntersectsLineOnSameAxis(a1, b1, b2) || pointIntersectsLineOnSameAxis(a2, b1, b2));
	}

	// return true if a line intersects with another line
	bool lineIntersectsLine(Point& p1, Point& p2, // first line
							Point& p3, Point& p4, // second line
							Point& intersect) // intersect point
	{
		int scenarioCode = 0;
		if (p1.x == p2.x)
			scenarioCode += 1;
		if (p1.y == p2.y)
			scenarioCode += 2;
		if (p3.x == p4.x)
			scenarioCode += 4;
		if (p3.y == p4.y)
			scenarioCode += 8;

		/*
		scenarioCode % 4 // state of line 1 (non-coplanar = 0, vertical = 1, horizontal = 2, point = 3)
		scenarioCode % 8 // state of line 2 (non-coplanar = 0, vertical = 1, horizontal = 2, point = 3)
		*/

		switch (scenarioCode)
		{
		case 0: { // non-coplanar : non-coplanar (confirmed completed)
			float slope1 = (p2.y - p1.y) / (p2.x - p1.x);
			float slope2 = (p4.y - p3.y) / (p4.x - p3.x);
			intersect.x = (p3.y - (slope2 * p3.x) - p1.y + (slope1 * p1.x)) / (slope1 - slope2);
			intersect.y = (-(slope2 * p1.y) + (slope1 * ((slope2 * p1.x) + p3.y - (slope2 * p3.x)))) / (slope1 - slope2);
			return (pointIntersectsLineOnSameAxis(intersect.x, p1.x, p2.x)
				 && pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p1.y, p2.y)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 1: { // vertical : non-coplanar (confirmed completed)
			float slope2 = (p4.y - p3.y) / (p4.x - p3.x);
			intersect.x = p1.x;
			intersect.y = (slope2 * intersect.x) + p3.y - (slope2 * p3.x);
			return (pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p1.y, p2.y)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 2: { // horizontal : non-coplanar (confirmed completed)
			float slope2 = (p4.y - p3.y) / (p4.x - p3.x);
			intersect.y = p1.y;
			intersect.x = (intersect.y - p3.y + (slope2 * p3.x)) / slope2;
			return (pointIntersectsLineOnSameAxis(intersect.x, p1.x, p2.x)
				 && pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 3: { // point : non-coplanar (confirmed completed)
			float slope2 = (p4.y - p3.y) / (p4.x - p3.x);
			intersect.x = p1.x;
			intersect.y = p1.y;
			return (intersect.y == ((slope2 * intersect.x) + p3.y - (slope2 * p3.x))
				 && pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 4: { // non-coplanar : vertical (confirmed completed)
			float slope1 = (p2.y - p1.y) / (p2.x - p1.x);
			intersect.x = p3.x;
			intersect.y = (slope1 * intersect.x) + p1.y - (slope1 * p1.x);
			return (pointIntersectsLineOnSameAxis(intersect.x, p1.x, p2.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p1.y, p2.y)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 5: { // vertical : vertical
			intersect.x = p1.x;
			intersect.y = 0.0f; // potentially multiple intersection points
			return ((p1.x == p3.x) && lineIntersectsLineOnSameAxis(p1.y, p2.y, p3.y, p4.y)); // didn't find intersection; figure this out later
		}
		case 6: { // horizontal : vertical (confirmed completed)
			intersect.x = p3.x;
			intersect.y = p1.y;
			return (pointIntersectsLineOnSameAxis(intersect.x, p1.x, p2.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 7: { // point : vertical (confirmed completed)
			intersect.x = p1.x;
			intersect.y = p1.y;
			return ((p1.x == p3.x) && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 8: { // non-coplanar : horizontal (confirmed completed)
			float slope1 = (p2.y - p1.y) / (p2.x - p1.x);
			intersect.y = p3.y;
			intersect.x = (intersect.y - p1.y + (slope1 * p1.x)) / slope1;
			return (pointIntersectsLineOnSameAxis(intersect.x, p1.x, p2.x)
				 && pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p1.y, p2.y));
		}
		case 9: { // vertical : horizontal (confirmed completed)
			intersect.x = p1.x;
			intersect.y = p3.y;
			return (pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p1.y, p2.y));
		}
		case 10: { // horizontal : horizontal
			intersect.x = 0.0f; // potentially multiple intersection points
			intersect.y = p1.y;
			return (lineIntersectsLineOnSameAxis(p1.x, p2.x, p3.x, p4.x) && (p1.y == p3.y)); // didn't find intersection; figure this out later
		}
		case 11: { // point : horizontal (confirmed completed)
			intersect.x = p1.x;
			intersect.y = p1.y;
			return (pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x) && (p1.y == p3.y));
		}
		case 12: { // non-coplanar : point (confirmed completed)
			float slope1 = (p2.y - p1.y) / (p2.x - p1.x);
			intersect.x = p3.x;
			intersect.y = p3.y;
			return (intersect.y == ((slope1 * intersect.x) + p1.y - (slope1 * p1.x))
				 && pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x)
				 && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 13: { // vertical : point (confirmed completed)
			intersect.x = p3.x;
			intersect.y = p3.y;
			return ((p1.x == p3.x) && pointIntersectsLineOnSameAxis(intersect.y, p3.y, p4.y));
		}
		case 14: { // horizontal : point (confirmed completed)
			intersect.x = p3.x;
			intersect.y = p3.y;
			return (pointIntersectsLineOnSameAxis(intersect.x, p3.x, p4.x) && (p1.y == p3.y));
		}
		case 15: { // point : point (confirmed completed)
			intersect.x = p3.x;
			intersect.y = p3.y;
			return ((p1.x == p3.x) && (p1.y == p3.y));
		}
		default: { // it shouldn't be possible to get here
			return 0;
		}
		}
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