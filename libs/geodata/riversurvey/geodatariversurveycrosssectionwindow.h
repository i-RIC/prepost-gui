#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_H

#include "gd_riversurvey_global.h"
#include "geodatarivercrosssection.h"

#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/base/qmainwindowwithsnapshot.h>

#include <QMainWindow>
#include <QList>

class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class HydraulicDataRiverSurveyWaterElevation;
class PreProcessorGeoDataGroupDataItemI;
class PreProcessorHydraulicDataGroupDataItemI;

class QAction;
class QIcon;
class QItemSelectionModel;
class QStandardItemModel;
class QTableView;
class QTableWidgetItem;

namespace Ui
{
	class GeoDataRiverSurveyCrosssectionWindow;
}

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCrosssectionWindow :
	public QMainWindowWithSnapshot,
	public AdditionalMenuWindowI
{
	Q_OBJECT

public:
	const static int TABLE_CHECK_WIDTH = 20;
	const static int TABLE_DATA_WIDTH = 160;
	const static int TABLE_NAME_WIDTH = 100;
	const static int TABLE_REALVAL_WIDTH = 60;
	const static int TABLE_COLOR_WIDTH = 40;
	const static int TABLE_ROWHEIGHT = 24;

	explicit GeoDataRiverSurveyCrosssectionWindow(PreProcessorGeoDataGroupDataItemI* gitem, GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~GeoDataRiverSurveyCrosssectionWindow();

	void setRiverSurvey(GeoDataRiverSurvey* rs);

	QString crosssectionName() const;
	void setCrosssection(const QString& name);
	GeoDataRiverPathPoint* target() const;
	QAction* addPointAction() const;
	QAction* deleteAction() const;
	QAction* editFromSelectedPointAction() const;
	QAction* editFromSelectedPointWithDialogAction() const;
	QAction* inactivateByWEOnlyThisAction() const;
	QAction* inactivateByWEAllAction() const;
	QAction* addVegetationAction() const;
	QAction* editSelectedVegetationAction() const;
	QAction* deleteSelectedVegetationAction() const;
	QAction* leftAddAction() const;
	QAction* leftSubAction() const;
	QAction* rightAddAction() const;
	QAction* rightSubAction() const;
	QAction* odnLeftStartAction() const;
	QAction* odnLeftMiddleAction() const;
	QAction* odnLeftLowAction() const;
	QAction* odnRightLowAction() const;
	QAction* odnRightMiddleAction() const;
	QAction* odnRightStartAction() const;

	void setupData();
	void updateSurveysTable();
	void updateWaterSurfaceElevationTable();
	void updateCrossSectionComboBox();
	void updateReferenceComboBox();
	void updateRiverSurveys();
	QTableView* tableView();
	bool canInactivateSelectedRows(GeoDataRiverCrosssection& cross, const std::vector<int>& indices);
	void informFocusIn();
	void toggleGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs);
	const QIcon& icon() const;
	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	QList<QMenu*> getAdditionalMenus() const override;
	const std::shared_ptr<QToolBar>& getAdditionalToolBar() const override;

	PreProcessorGeoDataGroupDataItemI* groupDataItem() const;
	GeoDataRiverSurveyCrosssectionWindowGraphicsView* graphicsView() const;
	void setSelectedRow(int row);
	bool isAspectRatioFixed() const;
	bool isRegionFixed() const;

public slots:
	void updateView();
	void cameraFit();
	void cameraMoveLeft();
	void cameraMoveRight();
	void cameraMoveUp();
	void cameraMoveDown();
	void cameraZoomIn();
	void cameraZoomOut();
	void cameraZoomInX();
	void cameraZoomOutX();
	void cameraZoomInY();
	void cameraZoomOutY();
	void update();

private slots:
	void updateActionStatus();
	void handleVegetationSelectionChange();
	void deleteSelectedRows();
	void editFromSelectedPoint();
	void editFromSelectedPointWithDialog();
	void inactivateByWEOnlyThis();
	void inactivateByWEAll();
	void enterAddPointMode();
	void enterAddVegetationMode();
	void editSelectedVegetation();
	void deleteSelectedVegetation();
	void startLeftAdd();
	void startLeftSub();
	void startRightAdd();
	void startRightSub();
	void setSelectedPointToOdnLeftStart();
	void setSelectedPointToOdnLeftMiddle();
	void setSelectedPointToOdnLeftLow();
	void setSelectedPointToOdnRightLow();
	void setSelectedPointToOdnRightMiddle();
	void setSelectedPointToOdnRightStart();
	void crosssectionComboBoxChange(int newindex);
	void handleDataChange();
	void handleVegetationDataChange();
	void handleSurveyTableItemEdit(QTableWidgetItem* item);
	void handleSurveyTableItemClick(QTableWidgetItem* item);
	void handleWseTableItemEdit(QTableWidgetItem* item);
	void handleWseTableItemClick(QTableWidgetItem* item);
	void handleSurveyTablecurrentCellChange(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void handleAspectRatioEdit(double ratio);
	void handleFixAspectRatio(bool fix);
	void handleFixRegion(bool fix);
	void handleDrawnRegionChanged();

	void moveUpWse(int index);
	void moveDownWse(int index);
	void deleteWse(int index);

	void editDisplaySetting();

private:
	static const int defaultRowHeight = 20;

	void setupSurveyTable();
	void setupWaterSurfaceElevationTable();
	bool syncData();
	bool syncVegetationData();
	void updateRiverPathPoints();
	void setupActions();
	void setupMenu();
	void clear();
	void setupToolBars();
	void setupModel();
	void setupView();
	void updateEditTargetPoint();
	bool selectWSEIndex(int* index);

	GeoDataRiverSurvey* targetRiverSurvey() const;
	GeoDataRiverSurvey* gridCreatingConditionRiverSurvey() const;
	GeoDataRiverPathPoint* gridCreatingConditionPoint() const;
	GeoDataRiverPathPoint* referenceRiverPathPoint() const;
	QColor referenceRiverPathPointColor() const;

	const QList<bool>& riverSurveyEnables() const;
	const QList<GeoDataRiverPathPoint*>& riverPathPoints() const;
	const QList<QColor>& riverSurveyColors() const;

	QStandardItemModel* model() const;
	QItemSelectionModel* selectionModel() const;
	QStandardItemModel* vegetationModel() const;
	QItemSelectionModel* vegetationSelectionModel() const;

	PreProcessorHydraulicDataGroupDataItemI* waterElevationGroup();
	HydraulicDataRiverSurveyWaterElevation* waterElevation(int index);

	Ui::GeoDataRiverSurveyCrosssectionWindow* ui;

	class Impl;
	Impl* impl;

	class DataTableDelegate;
	class JmkDataEditDialog;
	class PointAddDialog;
	class RiverSurveyTableDelegate;
	class VegetationDataTableDelegate;
	class WseTableDelegate;

public:
	friend class GeoDataRiverSurvey;
	friend class GeoDataRiverSurveyCrossSectionEditFromPointDialog;
	friend class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
};

#if _DEBUG
	#include "private/geodatariversurveycrosssectionwindow_impl.h"
#endif // _DEBUG

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
