#ifndef __vtkLegendBoxWidget_h
#define __vtkLegendBoxWidget_h

#include "guibase_global.h"
#include "vtkBorderWidget.h"

class vtkLegendBoxActor;
class vtkLegendBoxRepresentation;

class GUIBASEDLL_EXPORT vtkLegendBoxWidget : public vtkBorderWidget
{
public:
	static vtkLegendBoxWidget* New();
	vtkTypeRevisionMacro(vtkLegendBoxWidget, vtkBorderWidget);
	void PrintSelf(ostream& os, vtkIndent indent);

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
	virtual void CreateDefaultRepresentation();

protected:
	vtkLegendBoxWidget();
	~vtkLegendBoxWidget();

	int Repositionable;

	// Handle the case of Repositionable == 0
	static void MoveAction(vtkAbstractWidget*);

	// set the cursor to the correct shape based on State argument
	virtual void SetCursor(int State);

private:
	vtkLegendBoxWidget(const vtkLegendBoxWidget&);  //Not implemented
	void operator=(const vtkLegendBoxWidget&);  //Not implemented
};

#endif
