#ifndef __vtkLegendBoxWidget_h
#define __vtkLegendBoxWidget_h

#include "guibase_global.h"
#include <vtkBorderWidget.h>
#include <vtkVersionMacros.h>

class vtkLegendBoxActor;
class vtkLegendBoxRepresentation;

/// Widget for Legend box
class GUIBASEDLL_EXPORT vtkLegendBoxWidget : public vtkBorderWidget
{

public:
	static vtkLegendBoxWidget* New();
#if (VTK_MAJOR_VERSION == 6) && (VTK_MINOR_VERSION == 1)
	vtkTypeRevisionMacro(vtkLegendBoxWidget, vtkBorderWidget);
#else
	vtkTypeMacro(vtkLegendBoxWidget, vtkBorderWidget);
#endif
	void PrintSelf(ostream& os, vtkIndent indent) override;

	// Description:
	// Specify an instance of vtkWidgetRepresentation used to represent this
	// widget in the scene. Note that the representation is a subclass of vtkProp
	// so it can be added to the renderer independent of the widget.
	virtual void SetRepresentation(vtkLegendBoxRepresentation* rep);

	vtkLegendBoxRepresentation* GetLegendBoxRepresentation()
	{ return reinterpret_cast<vtkLegendBoxRepresentation*>(this->GetRepresentation()); }

	// Description:
	// Get the LegendBox used by this Widget. One is created automatically.
	virtual void SetLegendBoxActor(vtkLegendBoxActor* actor);
	virtual vtkLegendBoxActor* GetLegendBoxActor();

	// Description:
	// Can the widget be moved. On by default. If off, the widget cannot be moved
	// around.
	//
	// TODO: This functionality should probably be moved to the superclass.
	vtkSetMacro(Repositionable, int);
	vtkGetMacro(Repositionable, int);
	vtkBooleanMacro(Repositionable, int);

	// Description:
	// Create the default widget representation if one is not set.
	virtual void CreateDefaultRepresentation() override;

protected:
	vtkLegendBoxWidget();
	~vtkLegendBoxWidget();

	int Repositionable;

	// Handle the case of Repositionable == 0
	static void MoveAction(vtkAbstractWidget*);

	// set the cursor to the correct shape based on State argument
	virtual void SetCursor(int State) override;

private:
	vtkLegendBoxWidget(const vtkLegendBoxWidget&);  //Not implemented
	void operator=(const vtkLegendBoxWidget&);  //Not implemented
};

#endif
