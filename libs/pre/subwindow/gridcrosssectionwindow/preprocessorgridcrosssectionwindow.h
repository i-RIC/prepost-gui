#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW_H

//#include "geodatarivercrosssection.h"

#include <QMainWindow>
#include <QList>
#include <QUndoCommand>
#include <QIcon>

#include <vtkIdList.h>

class QComboBox;
class QRadioButton;
class QStandardItemModel;
class QItemSelectionModel;
class QModelIndex;
class QTableView;
class QAction;
class Structured2DGrid;
class PreProcessorGridCrosssectionWindowGraphicsView;
class PreProcessorGridCrosssectionWindowProjectDataItem;
class PreProcessorGridAttributeNodeDataItem;

namespace Ui
{
	class PreProcessorGridCrosssectionWindow;
}

//class GeoDataRiverSurveyCrosssectionEditCommand;

class PreProcessorGridCrosssectionWindow : public QMainWindow
{
	Q_OBJECT

public:
	const static int DEFAULT_TABLE_WIDTH;
	enum Direction {dirI, dirJ};
	explicit PreProcessorGridCrosssectionWindow(Structured2DGrid* g, const QString& condName, PreProcessorGridCrosssectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~PreProcessorGridCrosssectionWindow();
	void setTarget(Direction dir, int index);
	const QString& condition() const;
	Direction targetDirection() const;
	int targetIndex() const;
	void setupData();
	bool updateComboBoxes();
	QTableView* tableView() const;
	void informFocusIn();
	Structured2DGrid* grid() const;
	PreProcessorGridAttributeNodeDataItem* conditionNodeDataItem();
	void informSelectedVerticesChanged(const QVector<vtkIdType>& vertices);
	PreProcessorGridCrosssectionWindowProjectDataItem* projectDataItem() {return m_projectDataItem;}
	const QIcon& icon() const {return m_icon;}

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
	void blackComboBoxChange(int newindex);
	void redComboBoxChange(int newindex);
	void blueComboBoxChange(int newindex);
	void toggleI(bool toggled);
	void toggleJ(bool toggled);
	void handleDataChange();

private:
	bool syncData();
	static const int defaultRowHeight = 20;
	void setupActions();
	void clear();
	void setupToolBar();
	void setupModel();
	void setupView();
	Ui::PreProcessorGridCrosssectionWindow* ui;
	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;
	QIcon m_icon;

	Structured2DGrid* m_grid;
	QString m_condition;

	Direction m_direction;
	int m_blackLineIndex;
	int m_redLineIndex;
	int m_blueLineIndex;

	QRadioButton* m_iRadioButton;
	QRadioButton* m_jRadioButton;

	QComboBox* m_blackLineComboBox;
	QComboBox* m_redLineComboBox;
	QComboBox* m_blueLineComboBox;
	bool m_settingUp;
	PreProcessorGridCrosssectionWindowProjectDataItem* m_projectDataItem;
//	QList<GeoDataRiverPathPoint*> m_points;

public:
	friend class PreProcessorGridCrosssectionWindowGraphicsView;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW_H
