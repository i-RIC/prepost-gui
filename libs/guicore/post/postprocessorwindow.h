#ifndef POSTPROCESSORWINDOW_H
#define POSTPROCESSORWINDOW_H

#include "../guicore_global.h"
#include "../base/snapshotenabledwindowinterface.h"
#include <QMainWindow>
#include <QIcon>

class PostSolutionInfo;
class PostProcessorWindowProjectDataItem;
class PreProcessorGeoDataGroupDataItemInterface;
class ProjectData;

/// Abstract Post processor window
class GUICOREDLL_EXPORT PostProcessorWindow : public QMainWindow, public SnapshotEnabledWindowInterface
{
	Q_OBJECT

public:
	const static int MINWIDTH;
	const static int MINHEIGHT;

	PostProcessorWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi);
	virtual ~PostProcessorWindow();

	const QIcon& icon() const;
	int index() const;
	virtual void setupDefaultGeometry(int index) = 0;
	virtual void applyOffset(double x_diff, double y_diff);

public slots:
	// data is updated. re-render the graphics etc.
	virtual void update() {}

protected:
	PostSolutionInfo* postSolutionInfo();
	PreProcessorGeoDataGroupDataItemInterface* refDataGroupDataItem();

	PostProcessorWindowProjectDataItem* m_projectDataItem;
	int m_index;
	QIcon m_icon;
};

#endif // POSTPROCESSORWINDOW_H
