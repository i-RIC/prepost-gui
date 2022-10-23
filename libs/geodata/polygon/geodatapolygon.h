#ifndef GEODATAPOLYGON_H
#define GEODATAPOLYGON_H

#include "gd_polygon_global.h"

#include <geodata/polydata/geodatapolydata.h>

class GeoDataPolygonAbstractPolygon;
class GeoDataPolygonRegionPolygon;
class GeoDataPolygonHolePolygon;

class vtkActor;
class vtkCellArray;
class vtkMapper;
class vtkPoints;
class vtkPolyData;

class QAction;
class QMenu;
class QToolBar;

class QAction;
class QMenu;
class QPolygonF;
class QToolBar;

namespace geos {
namespace geom{

class Polygon;

} // geom
} // geos


class GD_POLYGON_EXPORT GeoDataPolygon : public GeoDataPolyData
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;

public:
	enum SelectMode {
		smNone,
		smPolygon
	};
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meTranslate,
		meTranslatePrepare,
		meMoveVertex,
		meMoveVertexPrepare,
		meAddVertex,
		meAddVertexPrepare,
		meAddVertexNotPossible,
		meRemoveVertexPrepare,
		meRemoveVertexNotPossible,

		meTranslateDialog,
		meEditVerticesDialog
	};

	GeoDataPolygon(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolygon() override;

	void setLineWidth(int lineWidth);

	void setupMenu() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;
	bool inNormalMode() const override;
	bool isDefined() const override;

	void definePolygon(bool doubleClick, bool noEditVal = false);

	void setShape(geos::geom::Polygon* polygon);
	void setShape(geos::geom::Polygon* polygon, const std::vector<unsigned int>& triangleCells);

	void clear();

	void showInitialDialog() override;

	void addHolePolygon(const QPolygonF& p);
	vtkPolyData* polyData() const;
	geos::geom::Polygon* getGeosPolygon(const QPointF& offset = QPointF(0, 0));

	GeoDataProxy* getProxy() override;
	void copyShape(GeoDataPolygon* polygon);

	MouseEventMode mouseEventMode() const;
	SelectMode selectMode() const;

	void setBCSettingMode(bool mode);

public slots:
	void restoreMouseEventMode();

private slots:
	void updatePolyData(bool noDraw = false);
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void editCoordinates();
	void addHolePolygon();
	void deletePolygon(bool force = false);
	void renderGraphics();
	void updatePolygon(GeoDataPolygon* polygon, vtkPoints* points, vtkCellArray* ca, bool noDraw);
	void copy();

private:
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void doApplyOffset(double x, double y) override;
	iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const override;

	void setMouseEventMode(MouseEventMode mode);
	void setSelectMode(SelectMode mode);

	bool checkCondition();
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateScalarValues() override;
	void updateActorSettings() override;
	bool selectObject(QPoint point);
	void deselectAll();
	bool activePolygonHasFourVertices();
	void updateMouseEventMode();
	void updateActionStatus();
	GeoDataPolygonHolePolygon* setupHolePolygon();
	void applyOffsetToAbstractPolygon(GeoDataPolygonAbstractPolygon* polygon, double x, double y);
	void clearHolePolygons();
	bool isReady() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	void lockMutex();
	void unlockMutex();

	void setupTriangleThread();

	GeoDataPolygonRegionPolygon* regionPolygon() const;
	GeoDataPolygonAbstractPolygon* selectedPolygon() const;
	void setSelectedPolygon(GeoDataPolygonAbstractPolygon* pol);
	const QList<GeoDataPolygonHolePolygon*>& holePolygons() const;
	QList<GeoDataPolygonHolePolygon*>& holePolygons();

	vtkActor* paintActor() const;

	QAction* addVertexAction() const;
	QAction* removeVertexAction() const;
	QAction* coordEditAction() const;
	QAction* holeModeAction() const;
	QAction* deleteAction() const;

	void setShapeUpdating(bool updating);

	class AddHolePolygonCommand;
	class AddVertexCommand;
	class PushNewPointCommand;
	class EditCoordinatesCommand;
	class EditValueCommand;
	class FinishPolygonDefiningCommand;
	class ModifyAbstractPolygonCommand;
	class MovePolygonCommand;
	class MoveVertexCommand;
	class RemoveVertexCommand;

	class CoordinatesEditor;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataPolygonAbstractPolygon;
	friend class GeoDataPolygonTriangleThread;
	friend class GeoDataPolygonGroup;
	friend class GeoDataPolygonGroupPolygon;
	friend class GeoDataPolygonImporter;
	friend class GeoDataPolygonShapeExporter;
	friend class GeoDataPolygonProxy;
	friend class PreProcessorBCSettingDataItem;
};

#endif // GEODATAPOLYGON_H
