#ifndef ABSTRACTCROSSSECTIONWINDOW_H
#define ABSTRACTCROSSSECTIONWINDOW_H

#include "crosssection_global.h"

#include <guicore/base/edgevisualizewindowi.h>

#include <QWidget>

namespace Ui {
class AbstractCrosssectionWindow;
}

class ColorMapSettingContainerI;
class v4Grid;
class v4Structured2dGrid;

class QDomNode;
class QMdiSubWindow;
class QTableView;
class QToolBar;
class QUndoCommand;
class QXmlStreamWriter;

class CROSSSECTIONDLL_EXPORT AbstractCrosssectionWindow : public QWidget, public EdgeVisualizeWindowI
{
	Q_OBJECT

public:
	enum class Mode {
		StructuredIJ,
		UnstructuredEdge,
	};

	enum class Direction {I, J};

	AbstractCrosssectionWindow(QWidget *parent);
	~AbstractCrosssectionWindow();

	virtual void loadFromProjectMainFile(const QDomNode& node);
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer);

	void setupDisplaySettings();
	void setTarget(Direction dir, int index);
	Direction targetDirection() const;
	int targetIndex() const;

	void applyTmpTargetSetting();
	void applyColorMapSetting(const std::string& name);
	QPixmap snapshot() const;
	QWidget* graphicsView() const;
	bool saveCsvFile(const QString& fileName);

	QToolBar* viewToolBar() const;
	QToolBar* displayToolBar() const;

	v4Structured2dGrid* targetGrid();
	v4Structured2dGrid* targetAdditionalGrid();

	virtual QMdiSubWindow* mdiSubWindow() const = 0;
	virtual v4Grid* grid() = 0;
	virtual v4Grid* additionalGrid();
	virtual QString additionalGridPrefix();

	class Controller;
	class GraphicsView;

	Controller* controller() const;

public slots:
	void handleGridReplace();
	void update();
	void handleSettingChange();
	void updateGraphicsView();
	void cameraFit();
	void openDisplaySettingDialog();
	void saveSnapshots();
	void exportCsvs();

protected:
	QTableView* editTable() const;

	class EditTableController;
	class GridAttributeDisplaySettingContainer;

	class Impl;
	Impl* impl;

	Ui::AbstractCrosssectionWindow *ui;

private:
	void pushUpdateCommand(QUndoCommand* command);
	void pushUpdateGraphicsViewCommand(QUndoCommand* command);

	virtual ColorMapSettingContainerI* preColorMapSetting(const std::string& name) const;

	class CsvExportController;
	class DisplaySettingContainer;
	class DisplaySettingDialog;
	class DisplaySettingTableController;
	class ExportSettingDialog;
	class GridAttributeDisplaySettingEditDialog;
	class SnapshotSaveController;
	class UpdateCommand;
	class UpdateGraphicsViewCommand;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_H
