#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15D_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15D_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkAppendPolyData.h>

#include <QVector>
#include <QVector2D>
#include <QPixmap>
#include <QCursor>

class QAction;
class QToolBar;
class RawDataRiverSurvey;
class RiverGridCtrlSolver;
//class GridCreatingConditionRiverSurvey15DPointAddDialog;
//class GridCreatingConditionRiverSurvey15DPointAddCommand;
//class GridCreatingConditionRiverSurvey15DPointRepositionDialog;
//class GridCreatingConditionCtrlPointRepositionCommand15D;

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
		RawDataRiverPathPoint* point;
		RawDataRiverPathPoint::CtrlZonePosition position;
		// only RawDataRiverPathPoint::zposCenterLine is valid.
		int index;
	};
	GridCreatingConditionRiverSurvey15D(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionRiverSurvey15D();
	bool create(QWidget* parent);

	bool ready() const;
	bool init();
	void setupActors();
	void setupMenu();
	void informSelection(PreProcessorGraphicsViewInterface* /*v*/);
	void informDeselection(PreProcessorGraphicsViewInterface* /*v*/);
	void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/);
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void enterEvent(QEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& /*range*/) override;
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	bool addToolBarButtons(QToolBar* /*tb*/) override;
	void createGrid(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end, int dataNum);
	void selectCreateRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end);
	bool checkCtrlPointsRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end);

	struct GridCtrlOffsetInfo {
		QVector2D direction;
		double length;
		double lowerLimit;
		double upperLimit;
	};
	GridCtrlOffsetInfo& GCPOffsetInfo() {return m_GCPOffsetInfo;}
	const GridCtrlOffsetInfo& GCPOffsetInfo() const {return m_GCPOffsetInfo;}
	void invalidateSelectedCtrlPoints();
	void clear();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void loadExternalData(const QString& filename);
	void saveExternalData(const QString& filename);

private:
	const static int LINEDIVS = 36;
	static const int ZONEDIV = 30;
	void selectCtrlZone(const QVector2D& point, double width);
	bool selectCtrlZone(RawDataRiverPathPoint* p, RawDataRiverPathPoint::CtrlZonePosition pos, const QVector2D& point, double width);

	void setupVtkContainers();
	void updateShapeData();
	void allActorsOff();

	bool ctrlPointSelectRegion(const QVector2D& p0, const QVector2D& v0, const QVector2D& v1);
	bool ctrlPointXORSelectRegion(const QVector2D& p0, const QVector2D& v0, const QVector2D& v1);

	void processCtrlPoints(int* index, Grid* grid, RawDataRiverPathPoint* p, int dataNum);
	void appendCrossSectionToGrid(RawDataRiverCrosssection& cs, Grid* grid, const QString& name);

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

	RawDataRiverSurvey* m_riverSurvey;

	RawDataRiverPathPoint* m_lastStartPoint;
	RawDataRiverPathPoint* m_lastEndPoint;

	RawDataRiverPathPoint* m_lastRegionAddStartPoint;
	RawDataRiverPathPoint* m_lastRegionAddEndPoint;

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
public:
	friend class GridCreatingConditionRiverSurvey15DPointAddDialog;
	friend class GridCreatingConditionRiverSurvey15DPointRepositionDialog;
	friend class GridCreatingConditionCreatorRiverSurvey15D;
	friend class GridCreatingConditionRiverSurvey15DPointAddCommand;
	friend class GridCreatingConditionCtrlPointRepositionCommand15D;
	friend class GridCreatingConditionCtrlPointMoveCommand15D;
	friend class GridCreatingConditionCtrlPointDeleteCommand15D;
	friend class GridCreatingConditionCtrlPointRegionAddCommand15D;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15D_H
