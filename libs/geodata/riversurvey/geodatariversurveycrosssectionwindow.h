#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_H

#include "gd_riversurvey_global.h"
#include "geodatarivercrosssection.h"

#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/additionalmenuwindowi.h>

#include <QMainWindow>
#include <QList>

class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class HydraulicDataRiverSurveyWaterElevation;
class PreProcessorGeoDataGroupDataItemInterface;
class PreProcessorHydraulicDataGroupDataItemInterface;

class QAction;
class QIcon;
class QItemSelectionModel;
class QTableView;
class QTableWidgetItem;

namespace Ui
{
	class GeoDataRiverSurveyCrosssectionWindow;
}

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCrosssectionWindow :
	public QMainWindow,
	public SnapshotEnabledWindowInterface,
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

	explicit GeoDataRiverSurveyCrosssectionWindow(PreProcessorGeoDataGroupDataItemInterface* gitem, GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~GeoDataRiverSurveyCrosssectionWindow();

	void setRiverSurvey(GeoDataRiverSurvey* rs);

	QString crosssectionName() const;
	void setCrosssection(const QString& name);
	GeoDataRiverPathPoint* target() const;
	QAction* deleteAction() const;
	QAction* editFromSelectedPointAction() const;
	QAction* editFromSelectedPointWithDialogAction() const;
	QAction* inactivateByWEOnlyThisAction() const;
	QAction* inactivateByWEAllAction() const;
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
	QPixmap snapshot() override;
	QList<QMenu*> getAdditionalMenus() const override;
	QToolBar* getAdditionalToolBar() const override;

	PreProcessorGeoDataGroupDataItemInterface* groupDataItem() const;
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
	void deleteSelectedRows();
	void editFromSelectedPoint();
	void editFromSelectedPointWithDialog();
	void inactivateByWEOnlyThis();
	void inactivateByWEAll();
	void crosssectionComboBoxChange(int newindex);
	void handleDataChange();
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

	QItemSelectionModel* selectionModel() const;
	PreProcessorHydraulicDataGroupDataItemInterface* waterElevationGroup();
	HydraulicDataRiverSurveyWaterElevation* waterElevation(int index);

	Ui::GeoDataRiverSurveyCrosssectionWindow* ui;

	class Impl;
	Impl* impl;

	class DataTableDelegate;
	class RiverSurveyTableDelegate;
	class WseTableDelegate;

public:
	friend class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
};

#if _DEBUG
	#include "private/geodatariversurveycrosssectionwindow_impl.h"
#endif // _DEBUG

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
