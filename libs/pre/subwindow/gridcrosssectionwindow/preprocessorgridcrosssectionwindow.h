#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW_H

#include <guicore/base/edgevisualizewindowi.h>
#include <guicore/base/qmainwindowwithsnapshot.h>

class PreProcessorGridCrosssectionInternalWindow;
class PreProcessorGridDataItem;

class PreProcessorGridCrosssectionWindow : public QMainWindowWithSnapshot, public EdgeVisualizeWindowI
{
	Q_OBJECT

public:
	PreProcessorGridCrosssectionWindow(PreProcessorGridDataItem* item, QWidget* parent);
	~PreProcessorGridCrosssectionWindow();

	PreProcessorGridCrosssectionInternalWindow* internalWindow() const;

	void handleGridReplace();
	void update();

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;
	void updateEdgeFocus() override;

public slots:
	void cameraFit();

private:
	PreProcessorGridCrosssectionInternalWindow* m_internalWindow;
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW_H
