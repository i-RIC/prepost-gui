#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDABSTRACTCELLSCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDABSTRACTCELLSCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selecteddatawithidcontroller.h"

class PreProcessorGridDataItem::SelectedAbstractCellsController : public SelectedDataWithIdController
{
public:
	SelectedAbstractCellsController(PreProcessorGridDataItem* item);
	~SelectedAbstractCellsController();

	void handleDataUpdate() override;
	void setVisibility(bool visible) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view) override;

	vtkActor* cellsActor() const;
	vtkActor* cellsLinesActor();

protected:
	virtual vtkPointSet* data() const = 0;
	virtual void updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view) = 0;
	void updateVtkObjects() override;

	vtkPolyData* m_emptyData;
	vtkExtractCells* m_extractCells;

	vtkDataSetMapper* m_cellsMapper;
	vtkActor* m_cellsActor;

	vtkDataSetMapper* m_cellsLinesMapper;
	vtkActor* m_cellsLinesActor;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDABSTRACTCELLSCONTROLLER_H
