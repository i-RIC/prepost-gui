#ifndef __vtkCustomStreamTracer_h
#define __vtkCustomStreamTracer_h

#include "guibase_global.h"

#include <vtkStreamTracer.h>

class GUIBASEDLL_EXPORT vtkCustomStreamTracer : public vtkStreamTracer
{
public:
	static vtkCustomStreamTracer *New();

	vtkSetMacro(MaximumIntegrationTime, double);
	vtkGetMacro(MaximumIntegrationTime, double);

protected:
	vtkCustomStreamTracer();
	~vtkCustomStreamTracer();

	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
	void Integrate(vtkPointData *inputData,
								 vtkPolyData* output,
								 vtkDataArray* seedSource,
								 vtkIdList* seedIds,
								 vtkIntArray* integrationDirections,
								 double lastPoint[3],
								 vtkAbstractInterpolatedVelocityField* func,
								 int maxCellSize,
								 int vecType,
								 const char *vecFieldName,
								 double& propagation,
								 vtkIdType& numSteps);
private:
	vtkCustomStreamTracer(const vtkCustomStreamTracer&);  // Not implemented.
	void operator=(const vtkCustomStreamTracer&);  // Not implemented.

	double MaximumIntegrationTime;
};

#endif
