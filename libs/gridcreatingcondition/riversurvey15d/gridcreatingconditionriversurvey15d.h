#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15D_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15D_H

#include "gridcreatingconditionriversurvey15dregiondialog.h"

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <geodata/riversurvey/geodatariverpathpoint.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkAppendPolyData.h>

#include <QCursor>
#include <QPixmap>
#include <QPointF>
#include <QVector>

class GeoDataRiverSurvey;
class v4InputGrid;
class v4Structured15dGridWithCrossSection;
class RiverGridCtrlSolver;

class QAction;
class QToolBar;

class GridCreatingConditionRiverSurvey15D : public GridCreatingCondition
{
	Q_OBJECT

private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
	const static int initialDivision = 100;

public:
	enum MouseEventMode {
		meNormal,
		meAddPointDialog,
		meMovePointDialog,
		meRepositionPointDialog,
		mePointRegionAddDialog,
		meCreationDialog,
	};
	struct ZoneSelectionInfo {
		GeoDataRiverPathPoint* point;
		GeoDataRiverPathPoint::CtrlZonePosition position;
		// only GeoDataRiverPathPoint::zposCenterLine is valid.
		int index;
	};
	GridCreatingConditionRiverSurvey15D(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionRiverSurvey15D();
	bool create(QWidget* parent) override;

	bool ready() const override;
	bool init() override;
	void setupActors() override;
	void setupMenu() override;
	void informSelection(PreProcessorGraphicsViewI* /*v*/) override;
	void informDeselection(PreProcessorGraphicsViewI* /*v*/) override;
	void viewOperationEnded(PreProcessorGraphicsViewI* /*v*/) override;
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/) override;
	void enterEvent(QEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/);
	void updateMouseCursor(PreProcessorGraphicsViewI* v);
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& /*range*/) override;
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	bool addToolBarButtons(QToolBar* /*tb*/) override;
	void createGrid(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end, int dataNum, GridCreatingConditionRiverSurvey15DRegionDialog::PositionMode positionMode);
	void selectCreateRegion(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end);
	bool checkCtrlPointsRegion(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end);

	struct GridCtrlOffsetInfo {
		QPointF direction;
		double length;
		double lowerLimit;
		double upperLimit;
	};
	GridCtrlOffsetInfo& GCPOffsetInfo() {return m_GCPOffsetInfo;}
	const GridCtrlOffsetInfo& GCPOffsetInfo() const {return m_GCPOffsetInfo;}
	void invalidateSelectedCtrlPoints();
	void clear() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

private:
	const static int LINEDIVS = 36;
	static const int ZONEDIV = 30;
	void selectCtrlZone(const QPointF& point, double width);
	bool selectCtrlZone(GeoDataRiverPathPoint* p, GeoDataRiverPathPoint::CtrlZonePosition pos, const QPointF& point, double width);

	void setupCrosssections(v4InputGrid* grid);

	void setupVtkContainers();
	void updateShapeData();
	void allActorsOff();

	bool ctrlPointSelectRegion(const QPointF& p0, const QPointF& v0, const QPointF& v1);
	bool ctrlPointXORSelectRegion(const QPointF& p0, const QPointF& v0, const QPointF& v1);

	void processCtrlPoints(int* index, v4Structured15dGridWithCrossSection* grid, GeoDataRiverPathPoint* p, int dataNum);
	void appendCrossSectionToGrid(GeoDataRiverCrosssection& cs, v4Structured15dGridWithCrossSection* grid, const QString& name);

	/// The pointdata, that has the positions of
	/// River center, left bank, and right bank
	vtkSmartPointer<vtkPoints> m_centerFixedPoints;
	vtkSmartPointer<vtkPoints> m_ctrlPointPoints;
	vtkSmartPointer<vtkPoints> m_selectedCtrlPointPoints;

	vtkSmartPointer<vtkStructuredGrid> m_riverCenterLine;

	vtkSmartPointer<vtkUnstructuredGrid> m_fixedPoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_ctrlPoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedCtrlPoints;

	vtkSmartPointer<vtkStructuredGrid> m_selectedCtrlZone;

	vtkSmartPointer<vtkAppendPolyData> m_createRegion;

	vtkSmartPointer<vtkLODActor> m_riverCenterLineActor;

	vtkSmartPointer<vtkActor> m_fixedPointsActor;
	vtkSmartPointer<vtkActor> m_blackFixedPointsActor;
	vtkSmartPointer<vtkActor> m_blackCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_ctrlPointsActor;
	vtkSmartPointer<vtkActor> m_selectedCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_blackSelectedCtrlPointsActor;

	vtkSmartPointer<vtkActor> m_selectedCtrlZoneActor;

	vtkSmartPointer<vtkActor> m_createRegionActor;

	void setupActions();
	void setActionStatus();
	bool isCtrlPointsContinuousSelection();

	GeoDataRiverSurvey* m_riverSurvey;

	GeoDataRiverPathPoint* m_lastStartPoint;
	GeoDataRiverPathPoint* m_lastEndPoint;

	GeoDataRiverPathPoint* m_lastRegionAddStartPoint;
	GeoDataRiverPathPoint* m_lastRegionAddEndPoint;
	GridCreatingConditionRiverSurvey15DRegionDialog::PositionMode m_positionMode;

	QAction* m_createAction;
	QAction* m_switchAction;
	QAction* m_addAction;
	QAction* m_addRegionallyAction;
	QAction* m_moveAction;
	QAction* m_repositionAction;
	QAction* m_deleteAction;

	QMenu* m_rightClickingMenu;
	QList<RiverGridCtrlSolver*> m_gridSolvers;
	QPoint m_dragStartPoint;

	std::list<CtrlPointSelectionInfo> m_selectedCtrlPointInfoList;

	GridCtrlOffsetInfo m_GCPOffsetInfo;

	ZoneSelectionInfo m_selectedZone;
	MouseEventMode m_mouseEventMode;

public slots:
	void addRegionPoints();
	void movePoints();
	void repositionPoints();
	void deletePoints();
	void hideCreateRegion();

private slots:
	void addPoints();
	void restoreMouseEventMode();
	void handleDataChange();
	void handleDataDestroy();

private:
	class DeleteCtrlPointCommand;

public:
	friend class GridCreatingConditionRiverSurvey15DPointAddDialog;
	friend class GridCreatingConditionRiverSurvey15DPointRepositionDialog;
	friend class GridCreatingConditionCreatorRiverSurvey15D;
	friend class GridCreatingConditionRiverSurvey15DPointAddCommand;
	friend class GridCreatingConditionCtrlPointRepositionCommand15D;
	friend class GridCreatingConditionCtrlPointMoveCommand15D;
	friend class GridCreatingConditionCtrlPointRegionAddCommand15D;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15D_H
