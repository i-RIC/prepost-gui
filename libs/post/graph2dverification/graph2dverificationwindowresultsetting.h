#ifndef GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H
#define GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H

#include <guicore/postcontainer/postsolutioninfo.h>

#include <qwt_symbol.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QColor>

#include <cgnslib.h>

class GeoDataPolyLine;
class MeasuredData;
class Graph2dVerificationWindowResultGroupDataItem;
class Graph2dVerificationWindowResultDataItem;
class Graph2dWindowDataItem;
class PostZoneDataContainer;
class ColorSource;
class QwtPlotCustomCurve;
class SolverDefinitionGridType;
class QDomNode;
class QXmlStreamWriter;

class Graph2dVerificationWindowResultSetting
{

public:
	enum XAxisMode {
		xaTime,
		xaI,
		xaJ,
		xaK,
		xaPolyline
	};

	enum DimType {
		dimBase,
		dim1D,
		dim2D,
		dim3D
	};

	enum GraphType {
		gtSWDvsValues,
		gtSWDvsError,
		gtMVvsCR,
		gtMVvsError
	};

	// constructor
	Graph2dVerificationWindowResultSetting();
	~Graph2dVerificationWindowResultSetting();

	bool init(PostSolutionInfo* sol, const std::vector<MeasuredData*>& measuredData, const QString& cgnsFilename);

	Graph2dVerificationWindowResultSetting& operator=(const Graph2dVerificationWindowResultSetting& s);

	PostSolutionInfo* postSolutionInfo() const { return m_postSolutionInfo; };
	std::vector<MeasuredData*> measuredData() const { return m_measuredData; };

	Graph2dVerificationWindowResultSetting::GraphType graphType() const { return m_graphType; };
	int timeStep() const { return m_timeStep; }
	PostZoneDataContainer* activePostData() const { return m_activePostData; };
	QString activeResult() const { return m_activeResult; };
	MeasuredData* activeMeasuredData() const { return m_activeMeasuredData; };
	QString activeValue() const { return m_activeValue; };
	QString activeFile() const { return m_activeFile; };
	QString activeZone() const { return m_activeZone; };

	void setActivePostData(PostZoneDataContainer* container);

	void setActiveMeasuredData(MeasuredData* activeMeasuredData) { m_activeMeasuredData = activeMeasuredData; };

	void setActiveValue(QString activeValue) { m_activeValue = activeValue; };

	void setActiveResult(QString activeResult) { m_activeResult = activeResult; };

	void setGraphType(GraphType graphType) { m_graphType = graphType; };

	void setTimeStep(int timestep) { m_timeStep = timestep; }

	void setActiveFile(QString activeFile) { m_activeFile = activeFile; }

	void setActiveZone(QString activeZone) { m_activeZone = activeZone; }

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

private:

	//friend class Graph2dVerificationWindowDataModel;

	PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	PostZoneDataContainer* m_activePostData;
	QString m_activeResult;

	MeasuredData* m_activeMeasuredData;
	GraphType m_graphType;

	int m_timeStep;
	QString m_activeValue;

	QString m_activeFile;
	QString m_activeZone;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H
