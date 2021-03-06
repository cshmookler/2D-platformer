#include "collision.h"

// eliminate small positional errors between the first position (p1) and the second position (p2)
void lgw::voidMinorPosDiff(Point& p1, Point& p2, float& errorMargin)
{
	if (std::abs(p1.x - p2.x) <= errorMargin)
		p1.x = p2.x;
	if (std::abs(p1.y - p2.y) <= errorMargin)
		p1.y = p2.y;
}

// return true if a point and a line segment on the ame axis intersect
bool lgw::pointIntersectsLineOnSameAxis(float a, float b1, float b2)
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
bool lgw::lineIntersectsLineOnSameAxis(float a1, float a2, float b1, float b2)
{
	return (pointIntersectsLineOnSameAxis(a1, b1, b2) || pointIntersectsLineOnSameAxis(a2, b1, b2));
}

// return true if a line intersects with another line
bool lgw::lineIntersectsLine(Point& p1, Point& p2, // first line
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
bool lgw::pointIntersectsObject(Point& p1, Point& p2, Point& p3)
{
	if (p2.x > p3.x)
	{
		if (p1.x > p2.x || p1.x < p3.x)
			return false;
	}
	else if (p2.x < p3.x)
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
bool lgw::pointIntersectsObject(Point& p1, Barrier2D& bar)
{
	return pointIntersectsObject(p1, bar.p1, bar.p2);
}
// return true if a point intersects with a 2D object
bool lgw::pointIntersectsObject(Point& p1, Object& obj)
{
	return pointIntersectsObject(p1, obj.p1, obj.p2);
}

// return true if a 2D object/barrier intersects with another 2D object/barrier
bool lgw::objectIntersectsObject(Point& p1, Point& p2, Point& p3, Point& p4)
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
bool lgw::objectIntersectsBarrier(Object& obj, Barrier2D& bar)
{
	return objectIntersectsObject(obj.p1, obj.p2, bar.p1, bar.p2);
}
// return true if a 2D object intersects with another 2D object
bool lgw::objectIntersectsObject(Object& obj1, Object& obj2)
{
	return objectIntersectsObject(obj1.p1, obj1.p2, obj2.p1, obj2.p2);
}