#ifndef TRIANGLEUTIL_H
#define TRIANGLEUTIL_H

#include "triangle_global.h"

struct triangulateio;

class TRIANGLEDLL_EXPORT TriangleUtil
{
public:
	static void clearTriangulateio(triangulateio* io);

private:
	TriangleUtil();
};

#endif // TRIANGLEUTIL_H
