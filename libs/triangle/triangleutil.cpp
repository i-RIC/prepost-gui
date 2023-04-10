#include "triangle.h"
#include "triangleutil.h"

void TriangleUtil::clearTriangulateio(triangulateio* io)
{
	io->pointlist = nullptr;
	io->pointattributelist = nullptr;
	io->pointmarkerlist = nullptr;
	io->numberofpoints = 0;
	io->numberofpointattributes = 0;
	io->trianglelist = nullptr;
	io->triangleattributelist = nullptr;
	io->trianglearealist = nullptr;
	io->neighborlist = nullptr;
	io->numberoftriangles = 0;
	io->numberofcorners = 0;
	io->numberoftriangleattributes = 0;
	io->segmentlist = nullptr;
	io->segmentmarkerlist = nullptr;
	io->numberofsegments = 0;
	io->holelist = nullptr;
	io->numberofholes = 0;
	io->regionlist = nullptr;
	io->numberofregions = 0;
	io->edgelist = nullptr;
	io->edgemarkerlist = nullptr;
	io->normlist = nullptr;
	io->numberofedges = 0;
}
