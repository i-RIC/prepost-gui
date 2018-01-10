#include "vtkCustomScalarBarActor.h"
#include "vtkScalarBarActorInternal.h"

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkScalarsToColors.h>
#include <vtkUnsignedCharArray.h>

vtkStandardNewMacro(vtkCustomScalarBarActor);

void vtkCustomScalarBarActor::ConfigureScalarBar()
{
	vtkScalarsToColors* lut = this->LookupTable;
	double* range = lut->GetRange();
	this->P->NumColors = lut->GetIndexedLookup() ?
		lut->GetNumberOfAnnotatedValues() : this->MaximumNumberOfColors;
	this->P->NumSwatches = this->P->NumColors + (this->DrawNanAnnotation ? 1 : 0);
	int numPts = 2 * (this->P->NumColors + 1) + (this->DrawNanAnnotation ? 4 : 0);
	this->P->SwatchPts = vtkPoints::New();
	this->P->SwatchPts->SetNumberOfPoints(numPts);
	this->P->Polys = vtkCellArray::New();
	this->P->Polys->Allocate(
		this->P->Polys->EstimateSize(this->P->NumSwatches,4));
	this->P->SwatchColors = vtkUnsignedCharArray::New();

	unsigned int nComponents = ((this->UseOpacity) ? 4 : 3);
	this->P->SwatchColors->SetNumberOfComponents(nComponents);
	this->P->SwatchColors->SetNumberOfTuples(this->P->NumSwatches);

	this->ScalarBarActor->SetProperty(this->GetProperty());
	this->ScalarBar->Initialize();
	this->ScalarBar->SetPoints(this->P->SwatchPts);
	this->ScalarBar->SetPolys(this->P->Polys);
	this->ScalarBar->GetCellData()->SetScalars(this->P->SwatchColors);
	this->P->SwatchPts->Delete();
	this->P->Polys->Delete();
	this->P->SwatchColors->Delete();

	double delta =
		static_cast<double>(this->P->ScalarBarBox.Size[1]) / this->P->NumColors;
	double x[3];
	x[2] = 0.;
	for (int i = 0; i < numPts / 2 - (this->DrawNanAnnotation ? 2 : 0); ++i)
		{
		x[this->P->TL[0]] = this->P->ScalarBarBox.Posn[this->P->TL[0]];
		x[this->P->TL[1]] = this->P->ScalarBarBox.Posn[this->P->TL[1]] + i * delta;
		this->P->SwatchPts->SetPoint(2 * i, x);

		x[this->P->TL[0]] = this->P->ScalarBarBox.Posn[this->P->TL[0]] +
			this->P->ScalarBarBox.Size[0];
		this->P->SwatchPts->SetPoint(2 * i + 1, x);
		}

	//polygons & cell colors
	unsigned char* rgb;
	double rgba[4];
	vtkIdType ptIds[4];
	for (int i = 0; i < this->P->NumColors; ++i)
		{
		ptIds[0] = 2 * i;
		ptIds[1] = ptIds[0] + 1;
		ptIds[2] = ptIds[1] + 2;
		ptIds[3] = ptIds[0] + 2;
		this->P->Polys->InsertNextCell(4,ptIds);
		double rgbval;
		if (this->LookupTable->UsingLogScale())
			{
			rgbval = log10(range[0]) +
				i * (log10(range[1]) - log10(range[0])) / this->P->NumColors;
			rgbval = pow(10.0, rgbval);
			}
		else
			{
			rgbval = range[0] + (range[1] - range[0]) *
				(i / static_cast<double>(this->P->NumColors - 1));
			}
		lut->GetColor(rgbval, rgba);
		rgba[3] = lut->GetOpacity(rgbval);
		//write into array directly
		rgb = this->P->SwatchColors->GetPointer(nComponents * i);
		rgb[0] = rgba[0] * 255.;
		rgb[1] = rgba[1] * 255.;
		rgb[2] = rgba[2] * 255.;
		if (this->P->SwatchColors->GetNumberOfComponents() > 3)
			{
			rgb[3] = this->UseOpacity ? rgba[3] * 255. : 255;
			}
		}

	// Set up a texture actor as an alternative to the 1-quad-per-color
	// scalar bar polydata.
	vtkPoints* texturePoints = vtkPoints::New();
	texturePoints->SetNumberOfPoints(4);
	this->TexturePolyData->SetPoints(texturePoints);
	texturePoints->SetPoint(0, 0.0, 0.0, 0.0);

	double p1[2], p2[2];
	p1[0] = this->P->ScalarBarBox.Posn[0];
	p1[1] = this->P->ScalarBarBox.Posn[1];
	p2[0] = p1[0] + this->P->ScalarBarBox.Size[this->P->TL[0]];
	p2[1] = p1[1] + this->P->ScalarBarBox.Size[this->P->TL[1]];

	texturePoints->SetPoint(0, p1[0], p1[1], 0.0);
	texturePoints->SetPoint(1, p2[0], p1[1], 0.0);
	texturePoints->SetPoint(2, p2[0], p2[1], 0.0);
	texturePoints->SetPoint(3, p1[0], p2[1], 0.0);
	texturePoints->Delete();

	double barWidth = this->P->ScalarBarBox.Size[this->P->TL[0]];
	double barHeight = this->P->ScalarBarBox.Size[this->P->TL[1]];
	vtkDataArray* tc = this->TexturePolyData->GetPointData()->GetTCoords();
	tc->SetTuple2(1, barWidth / this->TextureGridWidth, 0.0);
	tc->SetTuple2(2, barWidth / this->TextureGridWidth,
								barHeight / this->TextureGridWidth);
	tc->SetTuple2(3, 0.0, barHeight / this->TextureGridWidth);
}

vtkCustomScalarBarActor::vtkCustomScalarBarActor() :
	vtkScalarBarActor()
{}
