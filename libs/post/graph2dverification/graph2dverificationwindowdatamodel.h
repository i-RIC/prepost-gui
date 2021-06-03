#ifndef GRAPH2DTIMEWINDOWDATAMODEL_H
#define GRAPH2DTIMEWINDOWDATAMODEL_H

#include <post/graph2d/graph2dwindowdatamodel.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include "graph2dverificationwindowresultsetting.h"

#include <QVector>

class QwtPlotMarker;
class QwtPlotCustomCurve;

class Graph2dVerificationWindow;
class Graph2dVerificationWindowView;
class iRICMainWindow;
class PostSolutionInfo;

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

	/// Measured data
	const std::vector<MeasuredData*>& measuredData() const;

	bool setupInitialSetting();
	void getYAxisValueRange(Graph2dWindowDataModel::AxisSide as, double* min, double* max) override;
	const Graph2dVerificationWindowResultSetting& setting() const {return m_setting;}

public slots:
	void dataSourceSetting();

	void setType(int type);
	void exportData();

private slots:
	void updateTime();
	void updateData();
	void updateZoneList();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void applyAxisSetting() override {};

private:
	void updateGraph();
	void clearData();

	void init();
	void applySettings();
	const QList<PostZoneDataContainer*>& zoneContainers();

	QwtPlotMarker* m_timeMarker;
	Graph2dVerificationWindowResultSetting m_setting;

	int m_timeStart;
	int m_timeEnd;
	int m_timeSkip;

	///PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	QString m_xLabel;
	QString m_yLabel;

	QwtPlotCustomCurve* m_pointsCurve;
	QwtPlotCustomCurve* m_lineCurve;
	QwtPlotCustomCurve* m_dotLineCurve;
	QwtPlotMarker* m_zeroMarker;

	std::vector<double> xVals;
	std::vector<double> yVals;

	iRICMainWindow* m_iricMainWindow;

public:
	friend class Graph2dVerificationWindowProjectDataItem;
	friend class Graph2dVerificationWindowRootDataItem;
};

#endif // GRAPH2DTIMEWINDOWDATAMODEL_H
