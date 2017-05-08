#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_H

#include "gd_riversurvey_global.h"
#include "geodatariversurvey.h"
#include "geodatarivercrosssection.h"
#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/additionalmenuwindowi.h>
#include <guicore/project/colorsource.h>

#include <QMainWindow>
#include <QList>
#include <QIcon>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QUndoCommand>

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
class QComboBox;
class QStandardItemModel;
class QItemSelectionModel;
class QModelIndex;
class QTableView;
class QTableWidgetItem;
class QAction;
class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
class GeoDataRiverSurveyCrosssectionWindowProjectDataItem;
class PreProcessorGeoDataGroupDataItemInterface;

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
	GeoDataRiverPathPoint* target() const {return m_editTargetPoint;}
	QAction* deleteAction() const {return m_deleteAction;}
	QAction* inactivateByWEOnlyThisAction() const {return m_inactivateByWEOnlyThisAction;}
	QAction* inactivateByWEAllAction() const {return m_inactivateByWEAllAction;}
	void setupData();
	void updateSurveysTable();
	void updateComboBoxes();
	void updateRiverSurveys();
	QTableView* tableView();
	bool canInactivateSelectedRows(GeoDataRiverCrosssection& cross, QList<int> indices);
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
	Ui::GeoDataRiverSurveyCrosssectionWindow* ui;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	QMenu* m_elevationPointMenu;
	QAction* m_inactivateByWEOnlyThisAction;
	QAction* m_inactivateByWEAllAction;
	QAction* m_deleteAction;

	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;
	QString m_crosssectionName;
	GeoDataRiverSurvey* m_targetRiverSurvey;
	GeoDataRiverPathPoint* m_editTargetPoint;

	GeoDataRiverSurvey* m_gridCreatingConditionRiverSurvey;
	GeoDataRiverPathPoint* m_gridCreatingConditionPoint;

	QList<bool> m_riverSurveyEnables;
	QList<GeoDataRiverSurvey*> m_riverSurveys;
	QList<QColor> m_riverSurveyColors;
	QList<GeoDataRiverPathPoint*> m_riverPathPoints;
	QList<QString> m_crosssectionNames;

	QComboBox* m_crosssectionComboBox;

	bool m_settingUp;
	GeoDataRiverSurveyCrosssectionWindowProjectDataItem* m_projectDataItem;
	QList<GeoDataRiverPathPoint*> m_points;
	QIcon m_icon;
	ColorSource* m_colorSource;

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

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_H
