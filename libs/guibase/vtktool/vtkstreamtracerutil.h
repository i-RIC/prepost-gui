#ifndef VTKSTREAMTRACERUTIL_H
#define VTKSTREAMTRACERUTIL_H

#include "../guibase_global.h"

class vtkStreamTracer;
class vtkPoints;

class GUIBASEDLL_EXPORT vtkStreamTracerUtil
{
public:

	static void setupForParticleTracking(vtkStreamTracer* tracer);
	static void addParticlePointsAtTime(vtkPoints* points, vtkStreamTracer* tracer, double time);

private:
	vtkStreamTracerUtil();
};

#endif // VTKSTREAMTRACERUTIL_H
