#ifndef GRAPH2DTIMEWINDOWDATAMODEL_H
#define GRAPH2DTIMEWINDOWDATAMODEL_H

#include <post/graph2d/graph2dwindowdatamodel.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include "graph2dhybridwindowcontinuousexportdialog.h"
#include "graph2dhybridwindowresultsetting.h"

#include <QVector>

class QwtPlotMarker;
class PostSolutionInfo;
class Graph2dHybridWindow;
class Graph2dHybridWindowRootDataItem;
class Graph2dHybridWindowView;
class Graph2dHybridWindowObjectBrowser;
class Graph2dHybridWindowObjectBrowserView;
class Graph2dHybridWindowZoneDataItem;

/// This class handles ALL informations that are handled
/// inside 2d post-processing window.
class Graph2dHybridWindowDataModel : public Graph2dWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Graph2dHybridWindowDataModel(Graph2dHybridWindow* w, ProjectDataItem* parent);
	~Graph2dHybridWindowDataModel();
	/// The view to display 2d post-processor main region
	Graph2dHybridWindowView* view() const;
	PostSolutionInfo* postSolutionInfo();
	bool setupInitialSetting();
	void showSettingDialog();
	void getXAxisValueRange(double* min, double* max);
	void getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max) override;
	const Graph2dHybridWindowResultSetting& setting() const {return m_setting;}

public slots:
	void sliderChanged();
	void dataSourceSetting();
	void axisSetting();
	void drawSetting();
	void markerSettiing();
	void addKPMarkers();
	void specialSnapshot();
	void specialCsvExport();
	void copyCalculationResult();
	void exportCsv();
	void targetPolyLineDestroyed();
	void applySettingsSlot();
	
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
	template <typename DataItem> void getXY(DataItem* dataItem, QVector<double>* x, QVector<double>* y) const;

	QwtPlotMarker* m_timeMarker;
	Graph2dHybridWindowResultSetting m_setting;

	// parameters related to special snapshot.
	QString m_prefix;

	Graph2dHybridWindowContinuousExportDialog::RegionMode m_regionMode;
	int m_iMin;
	int m_iMax;
	int m_jMin;
	int m_jMax;
	int m_kMin;
	int m_kMax;
	int m_indexMin;
	int m_indexMax;

	Graph2dHybridWindowContinuousExportDialog::TimeMode m_timeMode;
	int m_timeStart;
	int m_timeEnd;
	int m_timeSkip;

	QString m_csvPrefix;

public:
	friend class Graph2dHybridWindowDataModelProjectDataItem;
	friend class Graph2dHybridWindowProjectDataItem;
	friend class Graph2dHybridWindowRootDataItem;
};

#endif // GRAPH2DTIMEWINDOWDATAMODEL_H
