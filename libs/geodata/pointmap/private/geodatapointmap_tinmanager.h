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

	void load(const QDomNode& node);
	void save(QXmlStreamWriter& writer);

	void removeBreakLine(BreakLine* line);
	std::vector<BreakLine*>& breakLines();
	const std::vector<BreakLine*>& breakLines() const;

	vtkPolyData* tin() const;
	vtkDoubleArray* values() const;
	void setTinData(vtkPolyData* data, vtkDoubleArray* values);
	void rebuildTinFromPointsIfNeeded();
	vtkPolyData* buildTinFromPoints();
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
	void showRemoveTrianglesSettingDialog();

private slots:
	void toggleAddBreakLineMode(bool on);
	void toggleRemoveBreakLineMode(bool on);

private:
	void setupActors();
	void rebuildQTree();

	class AddBreakLineController;
	class DeleteBreakLineController;
	class RemoveTrianglesSetting;
	class RemoveTrianglesSettingDialog;
	class RemoveTrianglesWithLongEdgeController;
	class TrianglesWithLongEdgeRemover;

	class Actions;
	Actions* m_actions;

	class Impl;
	Impl* impl;

	class RebuildTinFromPointsCommand;
	class RemoveTrianglesCommand;
	class TINBuilder;
};

#endif // GEODATAPOINTMAP_TINMANAGER_H
