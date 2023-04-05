#ifndef GEODATAPOINTMAP_TINMANAGER_H
#define GEODATAPOINTMAP_TINMANAGER_H

#include "../geodatapointmap.h"

#include <misc/mouseeventcontroller.h>

#include <QObject>

#include <unordered_set>

namespace geos {
	namespace index{
		namespace quadtree {
			class Quadtree;
		} // quadtree
	} // index
} // geos

class MouseEventController;

class vtkCell;
class vtkLODActor;

class GeoDataPointmap::TINManager : public QObject, public MouseEventController
{
	Q_OBJECT

public:
	class BreakLine;

	TINManager(PointsManager* points, GeoDataPointmap* pointmap);
	~TINManager();

	void removeBreakLine(BreakLine* line);
	std::vector<BreakLine*>& breakLines();
	const std::vector<BreakLine*>& breakLines() const;

	vtkPolyData* tin() const;
	vtkDoubleArray* values() const;
	void setTinData(vtkPolyData* data, vtkDoubleArray* values);
	void rebuildTinFromPointsIfNeeded();
	bool rebuildTinFromPoints(bool allowCancel);
	void updateBreakLinesActorSettings() const;

	bool needRebuild() const;
	void setNeedRebuild(bool needed);

	vtkCell* findCell(double x, double y, double* weights);
	bool checkIfBreakLinesHasIntersections() const;

	void setZDepthToBreakLines(double z);

	bool pointsUsedForBreakLines(const std::unordered_set<vtkIdType>& indices);
	void updateBreakLinesOnPointsDelete(const std::unordered_set<vtkIdType>& indices);
	void updateBreakLinesOnPointsUndoDelete(const std::unordered_set<vtkIdType>& indices);

	void addActorsToRenderer(vtkRenderer* renderer) override;
	void removeActorsFromRenderer(vtkRenderer* renderer) override;

	void addActionsToMenu(QMenu* menu) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

	vtkLODActor* tinActor() const;

public slots:
	void removeAllBreakLines();

private slots:
	void toggleAddBreakLineMode(bool on);
	void toggleRemoveBreakLineMode(bool on);

private:
	void setupActors();
	void rebuildQTree();

	std::vector<BreakLine*> m_breakLines;

	vtkPolyData* m_tin;
	vtkLODActor* m_tinActor;
	geos::index::quadtree::Quadtree* m_qTree;

	bool m_needRebuild;

	PointsManager* m_pointsManager;
	GeoDataPointmap* m_parent;

	class AddBreakLineController;
	class DeleteBreakLineController;
	class RemoveTrianglesWithLongEdgeController;

	MouseEventController* m_normalController;
	AddBreakLineController* m_addBreakLineController;
	DeleteBreakLineController* m_deleteBreakLineController;

	std::vector<MouseEventController*> m_controllers;
	MouseEventController* m_activeController;

	class Actions;
	Actions* m_actions;

	class RemoveTrianglesCommand;
	class TINBuilder;
};

#endif // GEODATAPOINTMAP_TINMANAGER_H
