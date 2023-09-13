#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_H

#include <guicore/base/qmainwindowwithsnapshot.h>

#include <QMainWindow>

namespace Ui {
class PreProcessorGridCrosssectionWindow2;
}

class PreProcessorGridDataItem;

class PreProcessorGridCrosssectionWindow2 : public QMainWindowWithSnapshot
{
	Q_OBJECT

public:
	enum class Direction {I, J};

	PreProcessorGridCrosssectionWindow2(PreProcessorGridDataItem* item, QWidget *parent);
	~PreProcessorGridCrosssectionWindow2();

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	void setTarget(Direction dir, int index);
	Direction targetDirection() const;
	int targetIndex() const;

	void applyTmpTargetSetting();
	void update();
	void applyColorMapSetting(const std::string& name);
	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	bool saveCsvFile(const QString& fileName);

	class GraphicsView;

public slots:
	void updateGraphicsView();
	void cameraFit();
	void openDisplaySettingDialog();
	void saveSnapshots();
	void exportCsvs();

private:
	void pushUpdateCommand(QUndoCommand* command);
	void pushUpdateGraphicsViewCommand(QUndoCommand* command);

	class Impl;
	Impl* impl;

	class Controller;
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

	Ui::PreProcessorGridCrosssectionWindow2 *ui;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_H
