#ifndef __vtkLegendBoxRepresentation_h
#define __vtkLegendBoxRepresentation_h

#include "guibase_global.h"

#include <vtkBorderRepresentation.h>

class vtkLegendBoxActor;

/// Container class for Legendbox representation
class GUIBASEDLL_EXPORT vtkLegendBoxRepresentation : public vtkBorderRepresentation
{

public:
	vtkTypeRevisionMacro(vtkLegendBoxRepresentation, vtkBorderRepresentation);
	virtual void PrintSelf(ostream& os, vtkIndent indent) override;
	static vtkLegendBoxRepresentation* New();

	// Description:
	// The prop that is placed in the renderer.
	vtkGetObjectMacro(LegendBoxActor, vtkLegendBoxActor);
	virtual void SetLegendBoxActor(vtkLegendBoxActor*);

	// Description:
	// Satisfy the superclass' API.
	virtual void BuildRepresentation() override;
	virtual void WidgetInteraction(double eventPos[2]) override;
	virtual void GetSize(double size[2])
	{size[0]=2.0; size[1]=2.0;}

	// Description:
	// These methods are necessary to make this representation behave as
	// a vtkProp.
	virtual void GetActors2D(vtkPropCollection* collection) override;
	virtual void ReleaseGraphicsResources(vtkWindow* window) override;
	virtual int RenderOverlay(vtkViewport*) override;
	virtual int RenderOpaqueGeometry(vtkViewport*) override;
	virtual int RenderTranslucentPolygonalGeometry(vtkViewport*) override;
	virtual int HasTranslucentPolygonalGeometry() override;

	// Description:
	// Get/Set the orientation.
	void SetOrientation(int orient);
	int GetOrientation();

protected:
	vtkLegendBoxRepresentation();
	~vtkLegendBoxRepresentation();

	vtkLegendBoxActor* LegendBoxActor;

private:
	vtkLegendBoxRepresentation(const vtkLegendBoxRepresentation&);  // Not implemented
	void operator=(const vtkLegendBoxRepresentation&);    // Not implemented
};

#endif //__vtkLegendBoxRepresentation_h
