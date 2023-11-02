#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW_H

#include <guicore/base/qmainwindowwithsnapshot.h>

class PreProcessorGridCrosssectionInternalWindow;
class PreProcessorGridDataItem;

class PreProcessorGridCrosssectionWindow : public QMainWindowWithSnapshot
{
	Q_OBJECT

public:
	PreProcessorGridCrosssectionWindow(PreProcessorGridDataItem* item, QWidget* parent);
	~PreProcessorGridCrosssectionWindow();

	PreProcessorGridCrosssectionInternalWindow* internalWindow() const;

	QPixmap snapshot() const override;
	QWidget* snapshotArea() const override;

public slots:
	void cameraFit();

private:
	PreProcessorGridCrosssectionInternalWindow* m_internalWindow;
	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW_H
