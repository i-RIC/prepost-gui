#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDCELLSCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDCELLSCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selectedabstractcellscontroller.h"

class PreProcessorGridDataItem::SelectedCellsController : public SelectedAbstractCellsController
{
public:
	SelectedCellsController(PreProcessorGridDataItem* item);
	~SelectedCellsController();

private:
	vtkPointSet* data() const override;
	void updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view) override;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDCELLSCONTROLLER_H
