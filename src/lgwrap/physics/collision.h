#pragma once

#include <iostream>
#include <cmath>
#include "object.h"

namespace lgw {
	// eliminate small positional errors between the first position (p1) and the second position (p2)
	void voidMinorPosDiff(Point& p1, Point& p2, float& errorMargin);

	// return true if a point and a line segment on the ame axis intersect
	bool pointIntersectsLineOnSameAxis(float a, float b1, float b2);

	// return true if two line segments on the same axis intersect
	bool lineIntersectsLineOnSameAxis(float a1, float a2, float b1, float b2);

	// return true if a line intersects with another line
	bool lineIntersectsLine(
		Point& p1, Point& p2, // first line
		Point& p3, Point& p4, // second line
		Point& intersect); // intersect point

	// return true if a point intersects with a 2D object/barrier defined by two points
	bool pointIntersectsObject(Point& p1, Point& p2, Point& p3);
	// return true if a point intersects with a 2D barrier
	bool pointIntersectsObject(Point& p1, Barrier2D& bar);
	// return true if a point intersects with a 2D object
	bool pointIntersectsObject(Point& p1, Object& obj);

	// return true if a 2D object/barrier intersects with another 2D object/barrier
	bool objectIntersectsObject(Point& p1, Point& p2, Point& p3, Point& p4);
	// return true if a 2D barrier intersects with a 2D object
	bool objectIntersectsBarrier(Object& obj, Barrier2D& bar);
	// return true if a 2D object intersects with another 2D object
	bool objectIntersectsObject(Object& obj1, Object& obj2);
}