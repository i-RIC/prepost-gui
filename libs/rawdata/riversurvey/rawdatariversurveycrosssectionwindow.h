#ifndef RAWDATARIVERSURVEYCROSSSECTIONWINDOW_H
#define RAWDATARIVERSURVEYCROSSSECTIONWINDOW_H

#include "rd_riversurvey_global.h"
#include "rawdatarivercrosssection.h"
#include <guicore/base/snapshotenabledwindow.h>
#include <guicore/base/additionalmenuwindow.h>
#include <guicore/project/colorsource.h>

#include <QMainWindow>
#include <QList>
#include <QIcon>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QUndoCommand>

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class QComboBox;
class QStandardItemModel;
class QItemSelectionModel;
class QModelIndex;
class QTableView;
class QTableWidgetItem;
class QAction;
class RawDataRiverSurveyCrosssectionWindowGraphicsView;
class RawDataRiverSurveyCrosssectionWindowProjectDataItem;
class PreProcessorRawDataGroupDataItemInterface;

namespace Ui
{
	class RawDataRiverSurveyCrosssectionWindow;
}

class RawDataRiverSurveyCrosssectionEditCommand;

class RD_RIVERSURVEY_EXPORT RawDataRiverSurveyCrosssectionWindow :
	public QMainWindow,
	public SnapshotEnabledWindow,
	public AdditionalMenuWindow
{
	Q_OBJECT
public:
	const static int TABLE_CHECK_WIDTH = 20;
	const static int TABLE_DATA_WIDTH = 160;
	const static int TABLE_COLOR_WIDTH = 40;
	const static int TABLE_ROWHEIGHT = 24;

	explicit RawDataRiverSurveyCrosssectionWindow(PreProcessorRawDataGroupDataItemInterface* gitem, RawDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~RawDataRiverSurveyCrosssectionWindow();
	void setRiverSurvey(RawDataRiverSurvey* rs);
	void setCrosssection(double name);
	RawDataRiverPathPoint* target() {return m_editTargetPoint;}
	QAction* deleteAction() {return m_deleteAction;}
	QAction* inactivateByWEOnlyThisAction() {return m_inactivateByWEOnlyThisAction;}
	QAction* inactivateByWEAllAction() {return m_inactivateByWEAllAction;}
	void setupData();
	void updateSurveysTable();
	void updateComboBoxes();
	void updateRiverSurveys();
	QTableView* tableView();
	bool canInactivateSelectedRows(RawDataRiverCrosssection& cross, QList<int> indices);
	void informFocusIn();
	void toggleGridCreatingMode(bool gridMode, RawDataRiverSurvey* rs);
	const QIcon& icon() const {return m_icon;}
	QPixmap snapshot();
	QList<QMenu*> getAdditionalMenus();
	PreProcessorRawDataGroupDataItemInterface* groupDataItem() const {return m_groupDataItem;}

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
	Ui::RawDataRiverSurveyCrosssectionWindow* ui;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;

	QMenu* m_elevationPointMenu;
	QAction* m_inactivateByWEOnlyThisAction;
	QAction* m_inactivateByWEAllAction;
	QAction* m_deleteAction;

	PreProcessorRawDataGroupDataItemInterface* m_groupDataItem;
	double m_crosssectionName;
	RawDataRiverSurvey* m_targetRiverSurvey;
	RawDataRiverPathPoint* m_editTargetPoint;

	RawDataRiverSurvey* m_gridCreatingConditionRiverSurvey;
	RawDataRiverPathPoint* m_gridCreatingConditionPoint;

	QList<bool> m_riverSurveyEnables;
	QList<RawDataRiverSurvey*> m_riverSurveys;
	QList<QColor> m_riverSurveyColors;
	QList<RawDataRiverPathPoint*> m_riverPathPoints;
	QList<double> m_crosssectionNames;

	QComboBox* m_crosssectionComboBox;

	bool m_settingUp;
	RawDataRiverSurveyCrosssectionWindowProjectDataItem* m_projectDataItem;
	QList<RawDataRiverPathPoint*> m_points;
	QIcon m_icon;
	ColorSource* m_colorSource;
public:
	friend class RawDataRiverSurveyCrosssectionWindowGraphicsView;
};

class RawDataRiverSurveyCrosssectionEditCommand : public QUndoCommand
{
public:
	RawDataRiverSurveyCrosssectionEditCommand(bool apply, const QString& title, RawDataRiverPathPoint* p, const RawDataRiverCrosssection::AltitudeList& after, const RawDataRiverCrosssection::AltitudeList& before, RawDataRiverSurveyCrosssectionWindow* w, RawDataRiverSurvey* rs, bool tableaction = false, QUndoCommand* parentcommand = nullptr);
	void redo();
	void undo();
private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;
	RawDataRiverPathPoint* m_point;
	RawDataRiverCrosssection::AltitudeList m_before;
	RawDataRiverCrosssection::AltitudeList m_after;
	RawDataRiverSurveyCrosssectionWindow* m_window;
	RawDataRiverSurvey* m_rs;
};

#endif // RAWDATARIVERSURVEYCROSSSECTIONWINDOW_H
