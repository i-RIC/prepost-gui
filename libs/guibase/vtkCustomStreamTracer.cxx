#include "vtkCustomStreamTracer.h"

#include <vtkAbstractInterpolatedVelocityField.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCompositeDataIterator.h>
#include <vtkCompositeDataSet.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkOverlappingAMR.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>

#include <vector>

vtkObjectFactoryNewMacro(vtkCustomStreamTracer)

namespace
{
	void InterpolatePoint(vtkDataSetAttributes* outPointData, vtkDataSetAttributes* inPointData,
		vtkIdType toId, vtkIdList *ids, double *weights, bool fast)
	{
		if (fast)
		{
			outPointData->InterpolatePoint(inPointData, toId, ids, weights);
		}
		else
		{
			for (int i = outPointData->GetNumberOfArrays() - 1; i >= 0; i--)
			{
				vtkAbstractArray* toArray = outPointData->GetAbstractArray(i);
				if (vtkAbstractArray* fromArray = inPointData->GetAbstractArray(toArray->GetName()))
				{
					toArray->InterpolateTuple(toId, ids, fromArray, weights);
				}
				else
				{
					outPointData->RemoveArray(toArray->GetName());
				}
			}
		}
	}
}

vtkCustomStreamTracer::vtkCustomStreamTracer() :
	vtkStreamTracer()
{}

vtkCustomStreamTracer::~vtkCustomStreamTracer()
{}

int vtkCustomStreamTracer::RequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector,
	vtkInformationVector *outputVector)
{
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	if (!this->SetupOutput(inInfo, outInfo))
	{
		return 0;
	}

	vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
	vtkDataSet *source = 0;
	if (sourceInfo)
	{
		source = vtkDataSet::SafeDownCast(
			sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
	}
	vtkPolyData *output = vtkPolyData::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkDataArray* seeds = 0;
	vtkIdList* seedIds = 0;
	vtkIntArray* integrationDirections = 0;
	this->InitializeSeeds(seeds, seedIds, integrationDirections, source);

	if (seeds)
	{
		double lastPoint[3];
		vtkAbstractInterpolatedVelocityField* func = 0;
		int maxCellSize = 0;
		if (this->CheckInputs(func, &maxCellSize) != VTK_OK)
		{
			vtkDebugMacro("No appropriate inputs have been found. Can not execute.");
			if (func)
			{
				func->Delete();
			}
			seeds->Delete();
			integrationDirections->Delete();
			seedIds->Delete();
			this->InputData->UnRegister(this);
			return 1;
		}

		if (vtkOverlappingAMR::SafeDownCast(this->InputData))
		{
			vtkOverlappingAMR* amr = vtkOverlappingAMR::SafeDownCast(this->InputData);
			amr->GenerateParentChildInformation();
		}

		vtkCompositeDataIterator* iter = this->InputData->NewIterator();
		vtkSmartPointer<vtkCompositeDataIterator> iterP(iter);
		iter->Delete();

		iterP->GoToFirstItem();
		vtkDataSet* input0 = 0;
		if (!iterP->IsDoneWithTraversal() && !input0)
		{
			input0 = vtkDataSet::SafeDownCast(iterP->GetCurrentDataObject());
			iterP->GoToNextItem();
		}
		int vecType(0);
		vtkDataArray *vectors = this->GetInputArrayToProcess(0, input0, vecType);
		if (vectors)
		{
			const char *vecName = vectors->GetName();
			double propagation = 0;
			vtkIdType numSteps = 0;
			this->Integrate(input0->GetPointData(), output,
				seeds, seedIds,
				integrationDirections,
				lastPoint, func,
				maxCellSize, vecType, vecName,
				propagation, numSteps);
		}
		func->Delete();
		seeds->Delete();
	}

	integrationDirections->Delete();
	seedIds->Delete();

	this->InputData->UnRegister(this);
	return 1;
}

void vtkCustomStreamTracer::Integrate(vtkPointData *input0Data,
																vtkPolyData* output,
																vtkDataArray* seedSource,
																vtkIdList* seedIds,
																vtkIntArray* integrationDirections,
																double lastPoint[3],
																vtkAbstractInterpolatedVelocityField* func,
																int maxCellSize,
																int vecType,
																const char *vecName,
																double& inPropagation,
																vtkIdType& inNumSteps)
{
	int i;
	vtkIdType numLines = seedIds->GetNumberOfIds();
	double propagation = inPropagation;
	vtkIdType numSteps = inNumSteps;

	// Useful pointers
	vtkDataSetAttributes* outputPD = output->GetPointData();
	vtkDataSetAttributes* outputCD = output->GetCellData();
	vtkPointData* inputPD;
	vtkDataSet* input;
	vtkDataArray* inVectors;

	int direction=1;

	if (this->GetIntegrator() == 0)
		{
		vtkErrorMacro("No integrator is specified.");
		return;
		}

	double* weights = 0;
	if ( maxCellSize > 0 )
		{
		weights = new double[maxCellSize];
		}

	// Used in GetCell()
	vtkGenericCell* cell = vtkGenericCell::New();

	// Create a new integrator, the type is the same as Integrator
	vtkInitialValueProblemSolver* integrator =
		this->GetIntegrator()->NewInstance();
	integrator->SetFunctionSet(func);

	// Since we do not know what the total number of points
	// will be, we do not allocate any. This is important for
	// cases where a lot of streamers are used at once. If we
	// were to allocate any points here, potentially, we can
	// waste a lot of memory if a lot of streamers are used.
	// Always insert the first point
	vtkPoints* outputPoints = vtkPoints::New();
	vtkCellArray* outputLines = vtkCellArray::New();

	// We will keep track of integration time in this array
	vtkDoubleArray* time = vtkDoubleArray::New();
	time->SetName("IntegrationTime");

	// This array explains why the integration stopped
	vtkIntArray* retVals = vtkIntArray::New();
	retVals->SetName("ReasonForTermination");

	vtkSmartPointer<vtkDoubleArray> velocityVectors;
	if(vecType != vtkDataObject::POINT)
		{
		velocityVectors = vtkSmartPointer<vtkDoubleArray>::New();
		velocityVectors->SetName(vecName);
		velocityVectors->SetNumberOfComponents(3);
		}
	vtkDoubleArray* cellVectors = 0;
	vtkDoubleArray* vorticity = 0;
	vtkDoubleArray* rotation = 0;
	vtkDoubleArray* angularVel = 0;
	if (this->ComputeVorticity)
		{
		cellVectors = vtkDoubleArray::New();
		cellVectors->SetNumberOfComponents(3);
		cellVectors->Allocate(3*VTK_CELL_SIZE);

		vorticity = vtkDoubleArray::New();
		vorticity->SetName("Vorticity");
		vorticity->SetNumberOfComponents(3);

		rotation = vtkDoubleArray::New();
		rotation->SetName("Rotation");

		angularVel = vtkDoubleArray::New();
		angularVel->SetName("AngularVelocity");
		}

	// We will interpolate all point attributes of the input on each point of
	// the output (unless they are turned off). Note that we are using only
	// the first input, if there are more than one, the attributes have to match.
	//
	// Note: We have to use a specific value (safe to employ the maximum number
	//       of steps) as the size of the initial memory allocation here. The
	//       use of the default argument might incur a crash problem (due to
	//       "insufficient memory") in the parallel mode. This is the case when
	//       a streamline intensely shuttles between two processes in an exactly
	//       interleaving fashion --- only one point is produced on each process
	//       (and actually two points, after point duplication, are saved to a
	//       vtkPolyData in vtkDistributedStreamTracer::NoBlockProcessTask) and
	//       as a consequence a large number of such small vtkPolyData objects
	//       are needed to represent a streamline, consuming up the memory before
	//       the intermediate memory is timely released.
	outputPD->InterpolateAllocate( input0Data,
																 this->MaximumNumberOfSteps );

	vtkIdType numPtsTotal=0;
	double velocity[3];

	int shouldAbort = 0;

	for(int currentLine = 0; currentLine < numLines; currentLine++)
		{

		double progress = static_cast<double>(currentLine)/numLines;
		this->UpdateProgress(progress);

		switch (integrationDirections->GetValue(currentLine))
			{
			case FORWARD:
				direction = 1;
				break;
			case BACKWARD:
				direction = -1;
				break;
			}

		// temporary variables used in the integration
		double point1[3], point2[3], pcoords[3], vort[3], omega;
		vtkIdType index, numPts=0;

		// Clear the last cell to avoid starting a search from
		// the last point in the streamline
		func->ClearLastCellId();

		// Initial point
		seedSource->GetTuple(seedIds->GetId(currentLine), point1);
		memcpy(point2, point1, 3*sizeof(double));
		if (!func->FunctionValues(point1, velocity))
			{
			continue;
			}

		if ( propagation >= this->MaximumPropagation ||
				 numSteps    >  this->MaximumNumberOfSteps)
			{
			continue;
			}

		numPts++;
		numPtsTotal++;
		vtkIdType nextPoint = outputPoints->InsertNextPoint(point1);
		time->InsertNextValue(0.0);

		// We will always pass an arc-length step size to the integrator.
		// If the user specifies a step size in cell length unit, we will
		// have to convert it to arc length.
		IntervalInformation stepSize;  // either positive or negative
		stepSize.Unit  = LENGTH_UNIT;
		stepSize.Interval = 0;
		IntervalInformation aStep; // always positive
		aStep.Unit = LENGTH_UNIT;
		double step, minStep=0, maxStep=0;
		double stepTaken, accumTime=0;
		double speed;
		double cellLength;
		int retVal=OUT_OF_LENGTH, tmp;

		// Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
		input = func->GetLastDataSet();
		inputPD = input->GetPointData();
		inVectors = input->GetAttributesAsFieldData(vecType)->GetArray(vecName);
		// Convert intervals to arc-length unit
		input->GetCell(func->GetLastCellId(), cell);
		cellLength = sqrt(static_cast<double>(cell->GetLength2()));
		speed = vtkMath::Norm(velocity);
		// Never call conversion methods if speed == 0
		if ( speed != 0.0 )
			{
			this->ConvertIntervals( stepSize.Interval, minStep, maxStep,
															direction, cellLength );
			}

		// Interpolate all point attributes on first point
		func->GetLastWeights(weights);
		InterpolatePoint(outputPD, inputPD, nextPoint, cell->PointIds, weights, this->HasMatchingPointAttributes);
		if(vecType != vtkDataObject::POINT)
			{
			velocityVectors->InsertNextTuple(velocity);
			}

		// Compute vorticity if required
		// This can be used later for streamribbon generation.
		if (this->ComputeVorticity)
			{
			if(vecType == vtkDataObject::POINT)
				{
				inVectors->GetTuples(cell->PointIds, cellVectors);
				func->GetLastLocalCoordinates(pcoords);
				vtkCustomStreamTracer::CalculateVorticity(cell, pcoords, cellVectors, vort);
				}
			else
				{
				vort[0] = 0;
				vort[1] = 0;
				vort[2] = 0;
				}
			vorticity->InsertNextTuple(vort);
			// rotation
			// local rotation = vorticity . unit tangent ( i.e. velocity/speed )
			if (speed != 0.0)
				{
				omega = vtkMath::Dot(vort, velocity);
				omega /= speed;
				omega *= this->RotationScale;
				}
			else
				{
				omega = 0.0;
				}
			angularVel->InsertNextValue(omega);
			rotation->InsertNextValue(0.0);
			}

		double error = 0;
		// Integrate until the maximum propagation length is reached,
		// maximum number of steps is reached or until a boundary is encountered.
		// Begin Integration
		while ( propagation < this->MaximumPropagation )
			{

			if (numSteps > this->MaximumNumberOfSteps)
				{
				retVal = OUT_OF_STEPS;
				break;
				}

			if ( numSteps++ % 1000 == 1 )
				{
				progress =
					( currentLine + propagation / this->MaximumPropagation ) / numLines;
				this->UpdateProgress(progress);

				if (this->GetAbortExecute())
					{
					shouldAbort = 1;
					break;
					}
				}

			// Never call conversion methods if speed == 0
			if ( (speed == 0) || (speed <= this->TerminalSpeed) )
				{
				retVal = STAGNATION;
				break;
				}

			// If, with the next step, propagation will be larger than
			// max, reduce it so that it is (approximately) equal to max.
			aStep.Interval = fabs( stepSize.Interval );

			if ( ( propagation + aStep.Interval ) > this->MaximumPropagation )
				{
				aStep.Interval = this->MaximumPropagation - propagation;
				if ( stepSize.Interval >= 0 )
					{
					stepSize.Interval = this->ConvertToLength( aStep, cellLength );
					}
				else
					{
					stepSize.Interval = this->ConvertToLength( aStep, cellLength ) * ( -1.0 );
					}
				maxStep = stepSize.Interval;
				}
			this->LastUsedStepSize = stepSize.Interval;

			// Calculate the next step using the integrator provided
			// Break if the next point is out of bounds.
			func->SetNormalizeVector( true );
			tmp = integrator->ComputeNextStep( point1, point2, 0, stepSize.Interval,
																				 stepTaken, minStep, maxStep,
																				 this->MaximumError, error );
			func->SetNormalizeVector( false );
			if ( tmp != 0 )
				{
				retVal = tmp;
				memcpy(lastPoint, point2, 3*sizeof(double));
				break;
				}

			// This is the next starting point
			for(i=0; i<3; i++)
				{
				point1[i] = point2[i];
				}

			// Interpolate the velocity at the next point
			if ( !func->FunctionValues(point2, velocity) )
				{
				retVal = OUT_OF_DOMAIN;
				memcpy(lastPoint, point2, 3*sizeof(double));
				break;
				}

			// It is not enough to use the starting point for stagnation calculation
			// Use average speed to check if it is below stagnation threshold
			double speed2 = vtkMath::Norm(velocity);
			if ( (speed+speed2)/2 <= this->TerminalSpeed )
				{
				retVal = STAGNATION;
				break;
				}

			accumTime += stepTaken / speed;
			// Calculate propagation (using the same units as MaximumPropagation
			propagation += fabs( stepSize.Interval );

			// Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
			input = func->GetLastDataSet();
			inputPD = input->GetPointData();
			inVectors = input->GetAttributesAsFieldData(vecType)->GetArray(vecName);


			// Point is valid. Insert it.
			numPts++;
			numPtsTotal++;
			nextPoint = outputPoints->InsertNextPoint(point1);
			time->InsertNextValue(accumTime);

			// Calculate cell length and speed to be used in unit conversions
			input->GetCell(func->GetLastCellId(), cell);
			cellLength = sqrt(static_cast<double>(cell->GetLength2()));
			speed = speed2;
			// Interpolate all point attributes on current point
			func->GetLastWeights(weights);
			InterpolatePoint(outputPD, inputPD, nextPoint, cell->PointIds, weights, this->HasMatchingPointAttributes);
			if(vecType != vtkDataObject::POINT)
				{
				velocityVectors->InsertNextTuple(velocity);
				}
			// Compute vorticity if required
			// This can be used later for streamribbon generation.
			if (this->ComputeVorticity)
				{
				if(vecType == vtkDataObject::POINT)
					{
					inVectors->GetTuples(cell->PointIds, cellVectors);
					func->GetLastLocalCoordinates(pcoords);
					vtkCustomStreamTracer::CalculateVorticity(cell, pcoords, cellVectors, vort);
					}
				else
					{
					vort[0] = 0;
					vort[1] = 0;
					vort[2] = 0;
					}
				vorticity->InsertNextTuple(vort);
				// rotation
				// angular velocity = vorticity . unit tangent ( i.e. velocity/speed )
				// rotation = sum ( angular velocity * stepSize )
				omega = vtkMath::Dot(vort, velocity);
				omega /= speed;
				omega *= this->RotationScale;
				index = angularVel->InsertNextValue(omega);
				rotation->InsertNextValue(rotation->GetValue(index-1) +
																	(angularVel->GetValue(index-1) + omega)/2 *
																	(accumTime - time->GetValue(index-1)));
				}

			// Never call conversion methods if speed == 0
			if ( (speed == 0) || (speed <= this->TerminalSpeed) )
				{
				retVal = STAGNATION;
				break;
				}

			// Convert all intervals to arc length
			this->ConvertIntervals( step, minStep, maxStep, direction, cellLength );


			// If the solver is adaptive and the next step size (stepSize.Interval)
			// that the solver wants to use is smaller than minStep or larger
			// than maxStep, re-adjust it. This has to be done every step
			// because minStep and maxStep can change depending on the cell
			// size (unless it is specified in arc-length unit)
			if (integrator->IsAdaptive())
				{
				if (fabs(stepSize.Interval) < fabs(minStep))
					{
					stepSize.Interval = fabs( minStep ) *
																stepSize.Interval / fabs( stepSize.Interval );
					}
				else if (fabs(stepSize.Interval) > fabs(maxStep))
					{
					stepSize.Interval = fabs( maxStep ) *
																stepSize.Interval / fabs( stepSize.Interval );
					}
				}
			else
				{
				stepSize.Interval = step;
				}

			// End Integration
			}

		if (shouldAbort)
			{
			break;
			}

		if (numPts > 1)
			{
			outputLines->InsertNextCell(numPts);
			for (i=numPtsTotal-numPts; i<numPtsTotal; i++)
				{
				outputLines->InsertCellPoint(i);
				}
			retVals->InsertNextValue(retVal);
			}

		// Initialize these to 0 before starting the next line.
		// The values passed in the function call are only used
		// for the first line.
		inPropagation = propagation;
		inNumSteps = numSteps;

		propagation = 0;
		numSteps = 0;
		}

	if (!shouldAbort)
		{
		// Create the output polyline
		output->SetPoints(outputPoints);
		outputPD->AddArray(time);
		if(vecType != vtkDataObject::POINT)
			{
			outputPD->AddArray(velocityVectors);
			}
		if (vorticity)
			{
			outputPD->AddArray(vorticity);
			outputPD->AddArray(rotation);
			outputPD->AddArray(angularVel);
			}

		vtkIdType numPts = outputPoints->GetNumberOfPoints();
		if ( numPts > 1 )
			{
			// Assign geometry and attributes
			output->SetLines(outputLines);
			if (this->GenerateNormalsInIntegrate)
				{
				this->GenerateNormals(output, 0, vecName);
				}

			outputCD->AddArray(retVals);
			}
		}

	if (vorticity)
		{
		vorticity->Delete();
		rotation->Delete();
		angularVel->Delete();
		}

	if (cellVectors)
		{
		cellVectors->Delete();
		}
	retVals->Delete();

	outputPoints->Delete();
	outputLines->Delete();

	time->Delete();


	integrator->Delete();
	cell->Delete();

	delete[] weights;

	output->Squeeze();
	return;
}
