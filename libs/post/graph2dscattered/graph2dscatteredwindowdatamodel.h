#ifndef GRAPH2DTIMEWINDOWDATAMODEL_H
#define GRAPH2DTIMEWINDOWDATAMODEL_H

#include <post/graph2d/graph2dwindowdatamodel.h>
#include <guicore/postcontainer/postsolutioninfo.h>
//#include "graph2dscatteredwindowcontinuousexportdialog.h"
#include "graph2dscatteredwindowresultsetting.h"

class QPixmap;
class QwtPlotMarker;
class PostSolutionInfo;
class Graph2dScatteredWindow;
class Graph2dScatteredWindowRootDataItem;
class Graph2dScatteredWindowView;
class Graph2dScatteredWindowObjectBrowser;
class Graph2dScatteredWindowObjectBrowserView;
class Graph2dScatteredWindowZoneDataItem;

/// This class handles ALL informations that are handled
/// inside 2d post-processing window.
class Graph2dScatteredWindowDataModel : public Graph2dWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Graph2dScatteredWindowDataModel(Graph2dScatteredWindow* w, ProjectDataItem* parent);
	~Graph2dScatteredWindowDataModel();
	/// The view to display 2d post-processor main region
	Graph2dScatteredWindowView* view() const;
	PostSolutionInfo* postSolutionInfo();
	bool setupInitialSetting();
	void getXAxisValueRange(double* min, double* max);
	void getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max) override;
	const Graph2dScatteredWindowResultSetting& setting() const {return m_setting;}

public slots:
//	void sliderChanged() override;
	void dataSourceSetting();
	void axisSetting();
	void drawSetting();

private slots:
	void updateTime();
	void updateData(int fn);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void applyAxisSetting() override;

private:
	void updateData();
	void init();
	void setupTimeMarker();
	void applySettings();
	bool savePixmap(const QPixmap& pixmap, const QString& filename);
	void showErrorMessage(const QString& filename);
	void updateTitle();
	const QList<PostZoneDataContainer*>& zoneContainers();
	QString formattedNumber(int number, int max);
	bool exportCsv(const QString& filename) const;

	QwtPlotMarker* m_timeMarker;
	Graph2dScatteredWindowResultSetting m_setting;

public:
	friend class Graph2dScatteredWindowDataModelProjectDataItem;
	friend class Graph2dScatteredWindowProjectDataItem;
	friend class Graph2dScatteredWindowRootDataItem;
};

#endif // GRAPH2DTIMEWINDOWDATAMODEL_H
