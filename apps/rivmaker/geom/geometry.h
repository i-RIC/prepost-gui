#ifndef GEOMETRY_H
#define GEOMETRY_H

class Rect;

class Geometry
{
public:
	virtual ~Geometry() {}

	virtual Rect rect() const = 0;
};

#endif // GEOMETRY_H
