#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDIEDGESCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDIEDGESCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selectedabstractcellscontroller.h"

class PreProcessorGridDataItem::SelectedIEdgesController : public SelectedAbstractCellsController
{
public:
	SelectedIEdgesController(PreProcessorGridDataItem* item);
	~SelectedIEdgesController();

private:
	vtkPointSet* data() const override;
	void updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view) override;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDIEDGESCONTROLLER_H
