#ifndef ABSTRACTCROSSSECTIONWINDOW_H
#define ABSTRACTCROSSSECTIONWINDOW_H

#include "crosssection_global.h"

#include <QWidget>

namespace Ui {
class AbstractCrosssectionWindow;
}

class ColorMapSettingContainerI;
class v4Structured2dGrid;

class QDomNode;
class QToolBar;
class QUndoCommand;
class QXmlStreamWriter;

class CROSSSECTIONDLL_EXPORT AbstractCrosssectionWindow : public QWidget
{
	Q_OBJECT

public:
	enum class Direction {I, J};

	AbstractCrosssectionWindow(QWidget *parent);
	~AbstractCrosssectionWindow();

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

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

	virtual v4Structured2dGrid* grid() = 0;
	virtual v4Structured2dGrid* additionalGrid();
	virtual QString additionalGridPrefix();

	class Controller;
	class GraphicsView;

	Controller* controller() const;

public slots:
	void update();
	void handleSettingChange();
	void updateGraphicsView();
	void cameraFit();
	void openDisplaySettingDialog();
	void saveSnapshots();
	void exportCsvs();

private:
	void pushUpdateCommand(QUndoCommand* command);
	void pushUpdateGraphicsViewCommand(QUndoCommand* command);

	virtual ColorMapSettingContainerI* preColorMapSetting(const std::string& name) const;

	class Impl;
	Impl* impl;

	class CsvExportController;
	class DisplaySettingContainer;
	class DisplaySettingDialog;
	class DisplaySettingTableController;
	class EditTableController;
	class ExportSettingDialog;
	class GridAttributeDisplaySettingContainer;
	class GridAttributeDisplaySettingEditDialog;
	class SnapshotSaveController;
	class UpdateCommand;
	class UpdateGraphicsViewCommand;

	Ui::AbstractCrosssectionWindow *ui;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_H
