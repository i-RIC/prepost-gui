#ifndef GRAPH2DTIMEWINDOWDATAMODEL_H
#define GRAPH2DTIMEWINDOWDATAMODEL_H

//{{
//#include <gui/verification/verificationgraphdialog.h>
//}}
#include <post/graph2d/graph2dwindowdatamodel.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include "graph2dverificationwindowcontinuousexportdialog.h"
#include "graph2dverificationwindowresultsetting.h"

#include <QVector>

class QwtPlotMarker;
class QwtPlotCustomCurve;

class PostSolutionInfo;
class Graph2dVerificationWindow;
class Graph2dVerificationWindowRootDataItem;
class Graph2dVerificationWindowView;
class Graph2dVerificationWindowObjectBrowser;
class Graph2dVerificationWindowObjectBrowserView;
class iRICMainWindow;
///class Graph2dHybridWindowZoneDataItem;

/// This class handles ALL informations that are handled
/// inside 2d post-processing window.
class Graph2dVerificationWindowDataModel : public Graph2dWindowDataModel
{
	Q_OBJECT

public:
	static const int MOVEWIDTH = 1;
	/// Constructor
	Graph2dVerificationWindowDataModel(Graph2dVerificationWindow* w, ProjectDataItem* parent);
	~Graph2dVerificationWindowDataModel();
	/// The view to display 2d post-processor main region
	Graph2dVerificationWindowView* view() const;
	PostSolutionInfo* postSolutionInfo();
	//{{
	/// Measured data
	const std::vector<MeasuredData*>& measuredData() const;
	//}}
	bool setupInitialSetting();
	void showSettingDialog();
	void getXAxisValueRange(double* min, double* max);
	void getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max) override;
	const Graph2dVerificationWindowResultSetting& setting() const {return m_setting;}

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
#if 0
	void exportCsv();
#endif
	void targetPolyLineDestroyed();
	void applySettingsSlot();

	void setType(int type);
	void exportData();

private slots:
	void updateTime();
	void updateData(int fn);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void applyAxisSetting() override;

private:
	void updateGraph();
	void clearData();

	void updateData();
	void init();
	//void setupTimeMarker();
	void applySettings();
	bool savePixmap(const QPixmap& pixmap, const QString& filename);
	void showErrorMessage(const QString& filename);
	void updateTitle();
	const QList<PostZoneDataContainer*>& zoneContainers();
	QString formattedNumber(int number, int max);
#if 0
	bool exportCsv(const QString& filename) const;
#endif
	template <typename DataItem> void getXY(DataItem* dataItem, QVector<double>* x, QVector<double>* y) const;

	QwtPlotMarker* m_timeMarker;
	Graph2dVerificationWindowResultSetting m_setting;

	// parameters related to special snapshot.
	QString m_prefix;

	Graph2dVerificationWindowContinuousExportDialog::RegionMode m_regionMode;
	int m_iMin;
	int m_iMax;
	int m_jMin;
	int m_jMax;
	int m_kMin;
	int m_kMax;
	int m_indexMin;
	int m_indexMax;

	Graph2dVerificationWindowContinuousExportDialog::TimeMode m_timeMode;
	int m_timeStart;
	int m_timeEnd;
	int m_timeSkip;

	QString m_csvPrefix;

	//{{
	///Graph2dVerificationWindowResultSetting::GraphType m_graphType;
	PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	///int m_timeStep;
	///PostZoneDataContainer* m_activePostData;
	///QString m_activeResult;

	///MeasuredData* m_activeMeasuredData;
	///QString m_activeValue;

	QString m_xLabel;
	QString m_yLabel;

	QwtPlotCustomCurve* m_pointsCurve;
	QwtPlotCustomCurve* m_lineCurve;
	QwtPlotCustomCurve* m_dotLineCurve;
	QwtPlotMarker* m_zeroMarker;

	std::vector<double> xVals;
	std::vector<double> yVals;

	////iRICMainWindow* m_mainWindow;
	iRICMainWindow* m_iricMainWindow;
	//}}

public:
	////friend class Graph2dHybridWindowDataModelProjectDataItem;
	friend class Graph2dVerificationWindowProjectDataItem;
	friend class Graph2dVerificationWindowRootDataItem;
};

#endif // GRAPH2DTIMEWINDOWDATAMODEL_H
