#include "vtklegendboxrepresentation.h"

#include <vtkObjectFactory.h>
#include <vtkPropCollection.h>
#include <vtkLegendBoxActor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

//=============================================================================
vtkCxxRevisionMacro(vtkLegendBoxRepresentation, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkLegendBoxRepresentation);
//-----------------------------------------------------------------------------
vtkLegendBoxRepresentation::vtkLegendBoxRepresentation()
{
	this->PositionCoordinate->SetValue(0.82, 0.1);
	this->Position2Coordinate->SetValue(0.17, 0.8);

	this->LegendBoxActor = NULL;
	vtkLegendBoxActor *actor = vtkLegendBoxActor::New();
	this->SetLegendBoxActor(actor);
	actor->Delete();

//  this->ShowBorder = vtkBorderRepresentation::BORDER_ACTIVE;
	this->BWActor->VisibilityOff();
}

//-----------------------------------------------------------------------------
vtkLegendBoxRepresentation::~vtkLegendBoxRepresentation()
{
	this->SetLegendBoxActor(NULL);
}

//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::SetLegendBoxActor(vtkLegendBoxActor* actor)
{
	if (this->LegendBoxActor != actor)
		{
		vtkSmartPointer<vtkLegendBoxActor> oldActor = this->LegendBoxActor;
		vtkSetObjectBodyMacro(LegendBoxActor, vtkLegendBoxActor, actor);
/*
		if (actor && oldActor)
			{
			actor->SetOrientation(oldActor->GetOrientation());
			}
*/
		}
}

//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::PrintSelf(ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "LegendBoxActor: " << this->LegendBoxActor << endl;
}

//-----------------------------------------------------------------------------
/*
void vtkLegendBoxRepresentation::SetOrientation(int orientation)
{
	if (this->LegendBoxActor)
		{
		this->LegendBoxActor->SetOrientation(orientation);
		}
}
*/
//-----------------------------------------------------------------------------
/*
int vtkLegendBoxRepresentation::GetOrientation()
{
	if (this->LegendBoxActor)
		{
		return this->LegendBoxActor->GetOrientation();
		}
	vtkErrorMacro("No scalar bar");
	return 0;
}
*/
//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::BuildRepresentation()
{
	if (this->LegendBoxActor)
		{
		this->LegendBoxActor->SetPosition(this->GetPosition());
		this->LegendBoxActor->SetPosition2(this->GetPosition2());
		}

	this->Superclass::BuildRepresentation();
}

//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::WidgetInteraction(double eventPos[2])
{
	// Let superclass move things around.
	this->Superclass::WidgetInteraction(eventPos);

/*
	// Check to see if we need to change the orientation.
	double *fpos1 = this->PositionCoordinate->GetValue();
	double *fpos2 = this->Position2Coordinate->GetValue();
	double par1[2];
	double par2[2];
	par1[0] = fpos1[0];
	par1[1] = fpos1[1];
	par2[0] = fpos1[0] + fpos2[0];
	par2[1] = fpos1[1] + fpos2[1];
	double center[2];
	center[0] = fpos1[0] + 0.5*fpos2[0];
	center[1] = fpos1[1] + 0.5*fpos2[1];
	bool orientationSwapped = false;
	if (fabs(center[0] - 0.5) > 0.2+fabs(center[1] - 0.5))
		{
		// Close enough to left/right to be swapped to vertical
		if (this->LegendBoxActor->GetOrientation() == VTK_ORIENT_HORIZONTAL)
			{
			this->LegendBoxActor->SetOrientation(VTK_ORIENT_VERTICAL);
			orientationSwapped = true;
			}
		}
	else if (fabs(center[1] - 0.5) > 0.2+fabs(center[0] - 0.5))
		{
		// Close enough to left/right to be swapped to horizontal
		if (this->LegendBoxActor->GetOrientation() == VTK_ORIENT_VERTICAL)
			{
			this->LegendBoxActor->SetOrientation(VTK_ORIENT_HORIZONTAL);
			orientationSwapped = true;
			}
		}

	if (orientationSwapped)
		{
		// Change the corners to effectively rotate 90 degrees.
		par2[0] = center[0] + center[1] - par1[1];
		par2[1] = center[1] + center[0] - par1[0];
		par1[0] = 2*center[0] - par2[0];
		par1[1] = 2*center[1] - par2[1];

		this->PositionCoordinate->SetValue(par1[0],par1[1]);
		this->Position2Coordinate->SetValue(par2[0] - par1[0], par2[1] - par1[1]);

		this->Modified();
		this->BuildRepresentation();
		}
*/
}

//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::GetActors2D(vtkPropCollection *collection)
{
	if (this->LegendBoxActor)
		{
		collection->AddItem(this->LegendBoxActor);
		}
	this->Superclass::GetActors2D(collection);
}

//-----------------------------------------------------------------------------
void vtkLegendBoxRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
	if (this->LegendBoxActor)
		{
		this->LegendBoxActor->ReleaseGraphicsResources(w);
		}
	this->Superclass::ReleaseGraphicsResources(w);
}

//-------------------------------------------------------------------------
int vtkLegendBoxRepresentation::RenderOverlay(vtkViewport *w)
{
	int count = this->Superclass::RenderOverlay(w);
	if (this->LegendBoxActor)
		{
		count += this->LegendBoxActor->RenderOverlay(w);
		}
	return count;
}

//-------------------------------------------------------------------------
int vtkLegendBoxRepresentation::RenderOpaqueGeometry(vtkViewport *w)
{
	int count = this->Superclass::RenderOpaqueGeometry(w);
	if (this->LegendBoxActor)
		{
		count += this->LegendBoxActor->RenderOpaqueGeometry(w);
		}
	return count;
}

//-------------------------------------------------------------------------
int vtkLegendBoxRepresentation::RenderTranslucentPolygonalGeometry(
																																 vtkViewport *w)
{
	int count = this->Superclass::RenderTranslucentPolygonalGeometry(w);
	if (this->LegendBoxActor)
		{
		count += this->LegendBoxActor->RenderTranslucentPolygonalGeometry(w);
		}
	return count;
}

//-------------------------------------------------------------------------
int vtkLegendBoxRepresentation::HasTranslucentPolygonalGeometry()
{
	int result = this->Superclass::HasTranslucentPolygonalGeometry();
	if (this->LegendBoxActor)
		{
		result |= this->LegendBoxActor->HasTranslucentPolygonalGeometry();
		}
	return result;
}

