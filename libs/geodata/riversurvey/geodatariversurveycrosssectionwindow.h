#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_H

#include "gd_riversurvey_global.h"
#include "geodatariversurvey.h"
#include "geodatarivercrosssection.h"

#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/additionalmenuwindowi.h>

#include <QMainWindow>
#include <QList>
#include <QUndoCommand>

class GeoDataRiverPathPoint;
class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class PreProcessorGeoDataGroupDataItemInterface;

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
	const static int TABLE_COLOR_WIDTH = 40;
	const static int TABLE_ROWHEIGHT = 24;

	explicit GeoDataRiverSurveyCrosssectionWindow(PreProcessorGeoDataGroupDataItemInterface* gitem, GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~GeoDataRiverSurveyCrosssectionWindow();

	void setRiverSurvey(GeoDataRiverSurvey* rs);
	void setCrosssection(const QString& name);
	GeoDataRiverPathPoint* target() const;
	QAction* deleteAction() const;
	QAction* inactivateByWEOnlyThisAction() const;
	QAction* inactivateByWEAllAction() const;
	void setupData();
	void updateSurveysTable();
	void updateComboBoxes();
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
	void inactivateByWEOnlyThis();
	void inactivateByWEAll();
	void crosssectionComboBoxChange(int newindex);
	void handleDataChange();
	void weCheckboxChange(bool checked);
	void weValueChange(double val);
	void handleSurveyTableItemEdit(QTableWidgetItem* item);
	void handleSurveyTableItemClick(QTableWidgetItem* item);
	void handleSurveyTablecurrentCellChange(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
	static const int defaultRowHeight = 20;

	void setupSurveyTable();
	bool syncData();
	void updateRiverPathPoints();
	void setupActions();
	void setupMenu();
	void clear();
	void setupToolBar();
	void setupModel();
	void setupView();
	void updateEditTargetPoint();

	GeoDataRiverSurvey* targetRiverSurvey() const;
	GeoDataRiverSurvey* gridCreatingConditionRiverSurvey() const;
	GeoDataRiverPathPoint* gridCreatingConditionPoint() const;

	const QList<bool>& riverSurveyEnables() const;
	const QList<GeoDataRiverPathPoint*>& riverPathPoints() const;
	const QList<QColor>& riverSurveyColors() const;

	QItemSelectionModel* selectionModel() const;

	Ui::GeoDataRiverSurveyCrosssectionWindow* ui;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
};

class GeoDataRiverSurvey::EditCrosssectionCommand : public QUndoCommand
{

public:
	EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction = false, QUndoCommand* parentcommand = nullptr);
	void redo() override;
	void undo() override;

private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	GeoDataRiverSurvey* m_rs;
};

#if _DEBUG
	#include "private/geodatariversurveycrosssectionwindow_impl.h"
#endif // _DEBUG

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
