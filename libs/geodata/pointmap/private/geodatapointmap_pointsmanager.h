#ifndef GEODATAPOINTMAP_POINTSMANAGER_H
#define GEODATAPOINTMAP_POINTSMANAGER_H

#include "../geodatapointmap.h"

#include <misc/mouseeventcontroller.h>

#include <QObject>

#include <unordered_set>
#include <vector>

class vtkActor;
class vtkLODActor;
class vtkPointLocator;
class vtkPolyDataMapper;

class GeoDataPointmap::PointsManager : public QObject, public MouseEventController
{
	Q_OBJECT

public:
	PointsManager(GeoDataPointmap* pointmap);
	~PointsManager();

	void setTinManager(TINManager* manager);
	void setSelectionPolygonZDepth(double z);
	void setPoints(vtkPoints* points, vtkDoubleArray* values);

	void setZDepthForAddAndInterpolatePoints(double z);

	vtkIdType closestPointWithinRadius(const QPointF& point, double radius);
	std::vector<vtkIdType> pointsWithinRadius(const QPointF& point, double radius);

	std::vector<vtkIdType> selectedPointIndices() const;
	std::unordered_set<vtkIdType> selectedPointIndicesUnorderedSet() const;
	void setSelectedPointIndices(const std::unordered_set<vtkIdType>& indices);

	void selectPointsInsideBox(MouseBoundingBox* box, bool xOr);
	void selectPointsInsidePolygon(const QPolygonF& polygon, bool xOr);
	void selectNearestPoint(const QPointF& position, double radius, bool xOr);
	void clearSelection();

	void addActorsToRenderer(vtkRenderer* renderer) override;
	void removeActorsFromRenderer(vtkRenderer* renderer) override;
	void addActorsForPointsMode(vtkActorCollection* col);

	void addActionsToMenu(QMenu* menu) override;

	void deactivate(VTK2DGraphicsView* v) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

	vtkPolyData* points() const;
	vtkDoubleArray* values() const;

	vtkLODActor* pointsActor() const;

	vtkMaskPoints* maskPoints10k() const;
	vtkMaskPoints* maskPoints40k() const;

	vtkPolyData* selectedPoints() const;
	vtkActor* selectedPointsActor() const;

	vtkPointLocator* pointLocator() const; // TODO remove this after refactoring

private slots:
	void toggleRectSelectMode(bool on);
	void togglePolygonSelectMode(bool on);
	void toggleAddPointsMode(bool on);
	void toggleInterpolatePointsMode(bool on);
	void editSelectedPoints();
	void deleteSelectedPoints();
	void filterSelectedPoints();
	void exportSelectedPoints();
	void remeshTinAndSwitchToTinEditMode();

private:
	void setupSettings();
	void switchToSelectPointBoundingBoxMode();
	void updateActionStatus();

	vtkPolyData* m_points;

	vtkLODActor* m_pointsActor;

	vtkMaskPoints* m_maskPoints10k;
	vtkMaskPoints* m_maskPoints40k;

	vtkPolyData* m_selectedPoints;
	vtkActor* m_selectedPointsActor;
	vtkPolyDataMapper* m_selectedPointsMapper;
	vtkPointLocator* m_pointLocator;

	TINManager* m_tinManager;
	GeoDataPointmap* m_parent;

	class EditPointsDialog;
	class FilterSelectedPointsDialog;

	class AddPointController;
	class InterpolatePointController;
	class SelectPointBoundingBoxController;
	class SelectPointPolygonController;

	SelectPointBoundingBoxController* m_selectPointBoundingBoxController;
	SelectPointPolygonController* m_selectPointPolygonController;
	AddPointController* m_addPointController;
	InterpolatePointController* m_interpolatePointController;

	std::vector<MouseEventController*> m_controllers;
	MouseEventController* m_activeController;

	class Actions;
	Actions* m_actions;

	class AddPointsCommand;
	class DeletePointsCommand;
	class EditSinglePointCommand;
	class EditPointsCommand;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_H
