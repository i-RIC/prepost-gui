#ifndef GRIDCREATINGCONDITIONRIVERSURVEY_H
#define GRIDCREATINGCONDITIONRIVERSURVEY_H

#include <rawdata/riversurvey/gridcreatingconditionriversurveyinterface.h>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkAppendPolyData.h>

#include <QVector>
#include <QVector2D>

#include <QPointF>
#include <QPixmap>
#include <QCursor>
#include <QUndoCommand>

class QAction;
class QToolBar;
class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class RiverGridCtrlSolver;
class GridCreatingConditionRiverSurveyPointAddDialog;
class GridCreatingConditionRiverSurveyPointAddCommand;
class GridCreatingConditionRiverSurveyPointRepositionDialog;
class GridCreatingConditionRiverSurveyPointMoveDialog;
class GridCreatingConditionCtrlPointRepositionCommand;
class GridCreatingConditionCtrlPointMoveCommand;

class GridCreatingConditionRiverSurvey : public GridCreatingConditionRiverSurveyInterface
{
	Q_OBJECT
private:
	const static int normalEdgeWidth = 1;
	const static int selectedEdgeWidth = 2;
	const static int initialDivision = 100;
public:
	enum MouseEventMode {
		meNormal,

		meMovePrepare,
		meMove,

		meAddPointDialog,
		meMovePointDialog,
		meRepositionPointDialog,
		mePointRegionAddDialog,
		meCreationDialog
	};
	struct ZoneSelectionInfo {
		RawDataRiverPathPoint* point;
		RawDataRiverPathPoint::CtrlZonePosition position;
		int index;
	};

	GridCreatingConditionRiverSurvey(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	virtual ~GridCreatingConditionRiverSurvey();
	bool create(QWidget* parent) override;

	bool ready() const override;
	bool init() override;
	void setupActors() override;
	void setupMenu() override;
	void informSelection(PreProcessorGraphicsViewInterface* /*v*/) override;
	void informDeselection(PreProcessorGraphicsViewInterface* /*v*/) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* /*v*/) override;
	void keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void enterEvent(QEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/);
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& /*range*/) override;
	void showDialog(QWidget* parent);
	void handleDialogAccepted(QDialog* d);
	void handleDialogRejected(QDialog* d);
	bool addToolBarButtons(QToolBar* /*tb*/) override;
	void createGrid(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end);
	void selectCreateRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end);
	bool checkCtrlPointsRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end);
	void updateShapeData() override;
	void updateShapeData(bool omitBackgroundUpdate);

	GridCtrlOffsetInfo& GCPOffsetInfo() override {return m_GCPOffsetInfo;}
	void invalidateSelectedCtrlPoints() override;
	void clear() override;
	std::list<CtrlPointSelectionInfo>& selectedCtrlPointInfoList() override {return m_selectedCtrlPointInfoList;}
	const std::list<CtrlPointSelectionInfo>& selectedCtrlPointInfoList() const {return m_selectedCtrlPointInfoList;}

	RawDataRiverPathPoint* lastStartPoint() const {return m_lastStartPoint;}
	RawDataRiverPathPoint* lastEndPoint() const {return m_lastEndPoint;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

private:
	const static int LINEDIVS = 16;
	static const int ZONEDIV = 10;
	void selectCtrlZone(const QVector2D& point, double width);
	bool selectCtrlZone(RawDataRiverPathPoint* p, RawDataRiverPathPoint::CtrlZonePosition pos, const QVector2D& point, double width);

	void setupVtkContainers();
	void updateGridInterpolators();
	void allActorsOff();
	void cancelBackgroundGridUpdate() override;

	bool ctrlPointSelectRegion(const QVector2D& p0, const QVector2D& v0, const QVector2D& v1);
	bool ctrlPointXORSelectRegion(const QVector2D& p0, const QVector2D& v0, const QVector2D& v1);
	void informCtrlPointUpdateToCrosssectionWindows() override;

	void setAnalogyCtrlPoints();

	/// The pointdata, that has the positions of
	/// River center, left bank, and right bank
	vtkSmartPointer<vtkPoints> m_crossSectionPoints;
	vtkSmartPointer<vtkPoints> m_ctrlPointPoints;
	vtkSmartPointer<vtkPoints> m_selectedCtrlPointPoints;
	vtkSmartPointer<vtkPoints> m_analogyCtrlPointPoints;

	vtkSmartPointer<vtkStructuredGrid> m_riverCenterLine;
	vtkSmartPointer<vtkStructuredGrid> m_leftBankLine;
	vtkSmartPointer<vtkStructuredGrid> m_rightBankLine;

	vtkSmartPointer<vtkUnstructuredGrid> m_crossSections;
	vtkSmartPointer<vtkUnstructuredGrid> m_fixedPoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_ctrlPoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedCtrlPoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_analogyCtrlPoints;

	vtkSmartPointer<vtkStructuredGrid> m_selectedCtrlZone;

	vtkSmartPointer<vtkAppendPolyData> m_createRegion;

	vtkSmartPointer<vtkLODActor> m_riverCenterLineActor;
	vtkSmartPointer<vtkLODActor> m_leftBankLineActor;
	vtkSmartPointer<vtkLODActor> m_rightBankLineActor;

	vtkSmartPointer<vtkActor> m_crossSectionsActor;
	vtkSmartPointer<vtkActor> m_fixedPointsActor;
	vtkSmartPointer<vtkActor> m_blackFixedPointsActor;
	vtkSmartPointer<vtkActor> m_blackCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_ctrlPointsActor;
	vtkSmartPointer<vtkActor> m_selectedCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_blackSelectedCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_analogyCtrlPointsActor;
	vtkSmartPointer<vtkActor> m_blackAnalogyCtrlPointsActor;

	vtkSmartPointer<vtkActor> m_selectedCtrlZoneActor;

	vtkSmartPointer<vtkActor> m_createRegionActor;

	void setupActions();
	void setActionStatus();
	bool isCtrlPointsContinuousSelection();

	RawDataRiverSurvey* m_riverSurvey;

	RawDataRiverPathPoint* m_lastStartPoint;
	RawDataRiverPathPoint* m_lastEndPoint;

	QString m_lastStartPointName;
	QString m_lastEndPointName;

	RawDataRiverPathPoint* m_lastRegionAddStartPoint;
	RawDataRiverPathPoint* m_lastRegionAddEndPoint;

	QAction* m_createAction;
	QAction* m_addAction;
	QAction* m_addRegionallyAction;
	QAction* m_moveAction;
	QAction* m_repositionAction;
	QAction* m_deleteAction;

	QMenu* m_rightClickingMenu;
	QList<RiverGridCtrlSolver*> m_gridSolvers;
	QPoint m_dragStartPoint;

	std::list<CtrlPointSelectionInfo> m_analogyCtrlPointInfoList;

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
	friend class GridCreatingConditionRiverSurveyPointAddDialog;
	friend class GridCreatingConditionRiverSurveyPointRepositionDialog;
	friend class GridCreatingConditionRiverSurveyPointMoveDialog;
	friend class GridCreatingConditionCreatorRiverSurvey;
	friend class GridCreatingConditionRiverSurveyPointAddCommand;
	friend class GridCreatingConditionCtrlPointRepositionCommand;
	friend class GridCreatingConditionCtrlPointMoveCommand;
	friend class GridCreatingConditionCtrlPointDeleteCommand;
	friend class GridCreatingConditionCtrlPointRegionAddCommand;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY_H
