#ifndef __iricActiveCellFilter_h
#define __iricActiveCellFilter_h

#include <vtkPolyDataAlgorithm.h>

class vtkIncrementalPointLocator;

// based on a combination of vtkGeometryFilter and vtkClipPolyData

class iricActiveCellFilter : public vtkPolyDataAlgorithm
{
public:
	static iricActiveCellFilter *New();
	vtkTypeMacro(iricActiveCellFilter, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Set the clipping value of the scalar value (if clipping with
	// scalars). The default value is 0.0.
	vtkSetMacro(Value, double);
	vtkGetMacro(Value, double);

	// Description:
	// Turn on/off selection of geometry by cell id.
	vtkSetMacro(CellClipping, int);
	vtkGetMacro(CellClipping, int);
	vtkBooleanMacro(CellClipping, int);

	// Description:
	// Set/Get the InsideOut flag. When off, a vertex is considered
	// inside the implicit function if its value is greater than the
	// Value ivar. When InsideOutside is turned on, a vertex is
	// considered inside the implicit function if its implicit function
	// value is less than or equal to the Value ivar.  InsideOut is off
	// by default.
	vtkSetMacro(InsideOut, int);
	vtkGetMacro(InsideOut, int);
	vtkBooleanMacro(InsideOut, int);

protected:
	iricActiveCellFilter();
	~iricActiveCellFilter();

	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	virtual int FillInputPortInformation(int port, vtkInformation *info);

	//special cases for performance
	void PolyDataExecute(vtkDataSet *, vtkPolyData *, vtkInformation *);
	void UnstructuredGridExecute(vtkDataSet *, vtkPolyData *, vtkInformation *);
	void StructuredGridExecute(vtkDataSet *, vtkPolyData *, vtkInformation *);
	int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	double Value;
	int CellClipping;
	int InsideOut;
private:
	iricActiveCellFilter(const iricActiveCellFilter&);  // Not implemented.
	void operator=(const iricActiveCellFilter&);  // Not implemented.
};

#endif


