#include "geometrypoint.h"
#include "rect.h"

Rect GeometryPoint::rect() const
{
	return Rect(x(), y());
}
