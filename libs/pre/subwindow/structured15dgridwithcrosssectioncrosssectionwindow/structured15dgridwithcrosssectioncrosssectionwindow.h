#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H

#include <guicore/grid/v4structured15dgridwithcrosssectioncrosssection.h>

#include <QMainWindow>
#include <QUndoCommand>

class v4Structured15dGridWithCrossSection;
class v4Structured15dGridWithCrossSectionCrossSection;
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
	Structured15DGridWithCrossSectionCrossSectionWindow(PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* pdi, QWidget* parent = nullptr);
	~Structured15DGridWithCrossSectionCrossSectionWindow();

	QAction* deleteAction() const;
	void setupData();
	bool updateComboBoxes();
	void setTarget(v4Structured15dGridWithCrossSectionCrossSection* cs);
	v4Structured15dGridWithCrossSectionCrossSection* target() const;

	class EditCommand;

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
	v4Structured15dGridWithCrossSection* m_grid;
	v4Structured15dGridWithCrossSectionCrossSection* m_blackLineCrossSection;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_shapeItem;
	bool m_settingUp;
	Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* m_projectDataItem;

	Ui::Structured15DGridWithCrossSectionCrossSectionWindow* ui;

public:
	friend class Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView;
	friend class Structured15DGridCrossSectionEditCommand;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_H
