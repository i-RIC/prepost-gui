#ifndef PREPROCESSORGRIDDATAITEM_SELECTEDEDGESCONTROLLER_H
#define PREPROCESSORGRIDDATAITEM_SELECTEDEDGESCONTROLLER_H

#include "../preprocessorgriddataitem.h"
#include "preprocessorgriddataitem_selecteddatacontroller.h"

#include <misc/edge.h>

class PreProcessorGridDataItem::SelectedEdgesController : public SelectedDataController
{
public:
	SelectedEdgesController(PreProcessorGridDataItem* item);
	~SelectedEdgesController();

	const std::vector<Edge>& selectedEdges() const;
	void setSelectedEdges(const std::vector<Edge>& edges);

	void handleDataUpdate() override;
	void setVisibility(bool visible) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view) override;
	void clearSelection() override;

	vtkActor* actor();

private:
	void updateSelectedEdges(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view);
	void updateVtkObjects() override;

	vtkPolyData* m_selectedData;
	vtkPolyDataMapper* m_mapper;
	vtkActor* m_actor;

	std::vector<Edge> m_selectedEdges;
};

#endif // PREPROCESSORGRIDDATAITEM_SELECTEDEDGESCONTROLLER_H
