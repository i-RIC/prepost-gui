/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCustomStreamPoints.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkCustomStreamPoints - generate points along streamer separated by constant time increment
// .SECTION Description
// vtkCustomStreamPoints is a filter that generates points along a streamer.
// The points are separated by a constant time increment. The resulting visual
// effect (especially when coupled with vtkGlyph3D) is an indication of
// particle speed.

// .SECTION See Also
// vtkStreamer vtkStreamLine vtkDashedStreamLine

#ifndef __vtkCustomStreamPoints_h
#define __vtkCustomStreamPoints_h

#include "guibase_global.h"
#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkStreamer.h"

/// Points used for particle tracking for vector (velocity~ fields
class GUIBASEDLL_EXPORT vtkCustomStreamPoints : public vtkStreamer
{

public:
	vtkTypeMacro(vtkCustomStreamPoints,vtkStreamer);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	// Description:
	// Construct object with time increment set to 1.0.
	static vtkCustomStreamPoints* New();

	// Description:
	// Specify the separation of points in terms of absolute time.
	vtkSetClampMacro(TimeIncrement,double,0.000001,VTK_DOUBLE_MAX);
	vtkGetMacro(TimeIncrement,double);

protected:
	vtkCustomStreamPoints();
	~vtkCustomStreamPoints() {}

	// Convert streamer array into vtkPolyData
	virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

	// the separation of points
	double TimeIncrement;

private:
	vtkCustomStreamPoints(const vtkCustomStreamPoints&);  // Not implemented.
	void operator=(const vtkCustomStreamPoints&);  // Not implemented.
};

#endif
