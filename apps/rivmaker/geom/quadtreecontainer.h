#ifndef QUADTREECONTAINER_H
#define QUADTREECONTAINER_H

#include <vector>

class Geometry;
class Rect;

class QuadtreeContainer
{
public:
	QuadtreeContainer();
	~QuadtreeContainer();

	void add(Geometry* geometry);
	void clear();

	std::vector<Geometry*> findIntersectRect(const Rect& rect);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/quadtreecontainer_impl.h"
#endif // _DEBUG

#endif // QUADTREECONTAINER_H
