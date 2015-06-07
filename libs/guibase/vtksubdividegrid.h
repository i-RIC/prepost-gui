#ifndef __VTKSUBDIVIDEGRID_H
#define __VTKSUBDIVIDEGRID_H

#include "guibase_global.h"

#include <vtkStructuredGridAlgorithm.h>

/// Algorithm to sumdivide structured grid cells
class GUIBASEDLL_EXPORT vtkSubdivideGrid : public vtkStructuredGridAlgorithm
{

public:
	static vtkSubdivideGrid* New();
	vtkTypeRevisionMacro(vtkSubdivideGrid,vtkStructuredGridAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	// Description:
	// Specify i-j-k (min,max) pairs to extract. The resulting structured grid
	// dataset can be of any topological dimension (i.e., point, line, plane,
	// or 3D grid).
	vtkSetVector6Macro(VOI,int);
	vtkGetVectorMacro(VOI,int,6);

	// Description:
	// Set the dividing rate in the i, j, and k directions. If the rate is > 1,
	// then the resulting VOI will be subsampled representation of the input.
	// For example, if the SampleRate=(2,2,2), every other point will be
	// selected, resulting in a volume 8th the original size.
	vtkSetVector3Macro(DivideRate, int);
	vtkGetVectorMacro(DivideRate, int, 3);

protected:
	vtkSubdivideGrid();
	~vtkSubdivideGrid() {}

	virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
	virtual int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
	virtual int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

	int VOI[6];
	int DivideRate[3];

private:
	vtkSubdivideGrid(const vtkSubdivideGrid&);  // Not implemented.
	void operator=(const vtkSubdivideGrid&);  // Not implemented.
};

#endif // __VTKSUBDIVIDEGRID_H
