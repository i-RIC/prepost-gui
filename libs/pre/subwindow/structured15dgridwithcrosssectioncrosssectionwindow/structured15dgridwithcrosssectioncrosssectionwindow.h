#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H

#include <guicore/pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.h>

#include <QMainWindow>
#include <QUndoCommand>

class Structured15DGridWithCrossSection;
class Structured15DGridWithCrossSectionCrossSection;
class Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem;
class PreProcessorNormal15DGridWithCrossSectionShapeDataItem;

class QStandardItemModel;
class QItemSelectionModel;
class QComboBox;

namespace Ui
{
	class Structured15DGridWithCrossSectionCrossSectionWindow;
}

class Structured15DGridWithCrossSectionCrossSectionWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit Structured15DGridWithCrossSectionCrossSectionWindow(PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~Structured15DGridWithCrossSectionCrossSectionWindow();
	QAction* deleteAction() {return m_deleteAction;}
	void setupData();
	bool updateComboBoxes();
	void setTarget(Structured15DGridWithCrossSectionCrossSection* cs);
	Structured15DGridWithCrossSectionCrossSection* target() {return m_blackLineCrossSection;}

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

private slots:
	void updateActionStatus();
	void deleteSelectedRows();
	void blackComboBoxChange(int newindex);
	void handleDataChange();

private:
	void syncData();
	static const int defaultRowHeight = 20;
	void setupActions();
	void clear();
	void setupToolBar();
	void setupModel();
	void setupView();

	QStandardItemModel* m_model;
	QItemSelectionModel* m_selectionModel;
	QAction* m_deleteAction;
	QComboBox* m_blackLineComboBox;
	Structured15DGridWithCrossSection* m_grid;
	Structured15DGridWithCrossSectionCrossSection* m_blackLineCrossSection;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_shapeItem;
	bool m_settingUp;
	Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* m_projectDataItem;

	Ui::Structured15DGridWithCrossSectionCrossSectionWindow* ui;

public:
	friend class Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView;
	friend class Structured15DGridCrossSectionEditCommand;
};

class Structured15DGridCrossSectionEditCommand : public QUndoCommand
{
public:
	Structured15DGridCrossSectionEditCommand(bool apply, const QString& title, const QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& after, const QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& before, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, bool tableaction = false);
	void redo();
	void undo();
private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;
//	RawDataRiverPathPoint* m_point;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude> m_before;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude> m_after;
	Structured15DGridWithCrossSectionCrossSectionWindow* m_window;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_item;
//	RawDataRiverSurvey* m_rs;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H
