#include "vtklegendboxrepresentation.h"
#include "vtklegendboxwidget.h"

#include <vtkCallbackCommand.h>
#include <vtkCoordinate.h>
#include <vtkLegendBoxActor.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

vtkCxxRevisionMacro(vtkLegendBoxWidget, "$Revision: 1.8 $");
vtkStandardNewMacro(vtkLegendBoxWidget);

//-------------------------------------------------------------------------
vtkLegendBoxWidget::vtkLegendBoxWidget()
{
	this->Selectable = 0;
	this->Repositionable = 1;

	// Override the subclasses callback to handle the Repositionable flag.
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
																					vtkWidgetEvent::Move,
																					this, vtkLegendBoxWidget::MoveAction);
}

//-------------------------------------------------------------------------
vtkLegendBoxWidget::~vtkLegendBoxWidget()
{
}

//-----------------------------------------------------------------------------
void vtkLegendBoxWidget::SetRepresentation(vtkLegendBoxRepresentation* rep)
{
	this->SetWidgetRepresentation(rep);
}

//-----------------------------------------------------------------------------
void vtkLegendBoxWidget::SetLegendBoxActor(vtkLegendBoxActor* actor)
{
	vtkLegendBoxRepresentation* rep = this->GetLegendBoxRepresentation();
	if (!rep) {
		this->CreateDefaultRepresentation();
		rep = this->GetLegendBoxRepresentation();
	}

	if (rep->GetLegendBoxActor() != actor) {
		rep->SetLegendBoxActor(actor);
		this->Modified();
	}
}

//-----------------------------------------------------------------------------
vtkLegendBoxActor* vtkLegendBoxWidget::GetLegendBoxActor()
{
	vtkLegendBoxRepresentation* rep = this->GetLegendBoxRepresentation();
	if (!rep) {
		this->CreateDefaultRepresentation();
		rep = this->GetLegendBoxRepresentation();
	}

	return rep->GetLegendBoxActor();
}

//-----------------------------------------------------------------------------
void vtkLegendBoxWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep) {
		vtkLegendBoxRepresentation* rep = vtkLegendBoxRepresentation::New();
		this->SetRepresentation(rep);
		rep->Delete();
	}
}

//-------------------------------------------------------------------------
void vtkLegendBoxWidget::SetCursor(int cState)
{
	if (!this->Repositionable && !this->Selectable
			&& cState == vtkBorderRepresentation::Inside) {
		// Don't have a special cursor for the inside if we cannot reposition.
		this->RequestCursorShape(VTK_CURSOR_DEFAULT);
	} else {
		this->Superclass::SetCursor(cState);
	}
}

//-------------------------------------------------------------------------
void vtkLegendBoxWidget::MoveAction(vtkAbstractWidget* w)
{
	// The the superclass handle most stuff.
	vtkLegendBoxWidget::Superclass::MoveAction(w);

	vtkLegendBoxWidget* self = reinterpret_cast<vtkLegendBoxWidget*>(w);
	vtkLegendBoxRepresentation* representation=self->GetLegendBoxRepresentation();

	// Handle the case where we suppress widget translation.
	if (!self->Repositionable
			&& (representation->GetInteractionState()
					== vtkBorderRepresentation::Inside)) {
		representation->MovingOff();
	}
}

//-------------------------------------------------------------------------
void vtkLegendBoxWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "Repositionable: " << this->Repositionable << endl;
}
