#include "vtk2dinteractorstyle.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(vtk2DInteractorStyle);

vtk2DInteractorStyle::vtk2DInteractorStyle(void)
{}

vtk2DInteractorStyle::~vtk2DInteractorStyle(void)
{}

void vtk2DInteractorStyle::OnLeftButtonDown()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
													this->Interactor->GetEventPosition()[1]);
	if (this->CurrentRenderer == NULL) {
		return;
	}

	this->GrabFocus((vtkCommand*) this->EventCallbackCommand);
	if (this->Interactor->GetShiftKey()) {
		if (this->Interactor->GetControlKey()) {
			this->StartSpin();
		} else {
			this->StartPan();
		}
	} else {
		if (this->Interactor->GetControlKey()) {
			this->StartDolly();
		} else {
			this->StartSpin();
		}
	}
}
