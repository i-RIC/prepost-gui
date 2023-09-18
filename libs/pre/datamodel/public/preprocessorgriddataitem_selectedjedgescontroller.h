#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDJEDGESCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDJEDGESCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selectedabstractcellscontroller.h"

class PreProcessorGridDataItem::SelectedJEdgesController : public SelectedAbstractCellsController
{
public:
	SelectedJEdgesController(PreProcessorGridDataItem* item);
	~SelectedJEdgesController();

private:
	vtkPointSet* data() const override;
	void updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view) override;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDJEDGESCONTROLLER_H
