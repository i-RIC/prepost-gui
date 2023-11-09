#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDNODESCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDNODESCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selecteddatawithidcontroller.h"

class PreProcessorGridDataItem::SelectedNodesController : public SelectedDataWithIdController
{
public:
	SelectedNodesController(PreProcessorGridDataItem* item);
	~SelectedNodesController();

	void handleDataUpdate() override;
	void setVisibility(bool visible) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view) override;

	vtkActor* actor() const;

	std::unordered_set<vtkIdType> getSelectedVertices(MouseBoundingBox* box, bool xOr);

private:
	void updateSelectedVerticesBySelectingNearest(QPointF& pos, double maxDistance, bool xOr);
	void updateSelectedVertices(MouseBoundingBox* box, bool xOr);
	void updateVtkObjects() override;

	vtkPolyData* m_selectedData;
	vtkPolyDataMapper* m_mapper;
	vtkActor* m_actor;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDNODESCONTROLLER_H
