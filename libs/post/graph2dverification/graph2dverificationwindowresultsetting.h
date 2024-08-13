#ifndef GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H
#define GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H

#include <guicore/postcontainer/postsolutioninfo.h>

#include <qwt_symbol.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QColor>

class ColorSource;
class GeoDataPolyLine;
class Graph2dVerificationWindowResultGroupDataItem;
class Graph2dVerificationWindowResultDataItem;
class Graph2dWindowDataItem;
class MeasuredData;
class SolverDefinitionGridType;
class v4PostZoneDataContainer;

class QwtPlotCustomCurve;

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

	Graph2dVerificationWindowResultSetting();
	~Graph2dVerificationWindowResultSetting();

	bool init(PostSolutionInfo* sol, const std::vector<MeasuredData*>& measuredData);

	Graph2dVerificationWindowResultSetting& operator=(const Graph2dVerificationWindowResultSetting& s);

	PostSolutionInfo* postSolutionInfo() const;
	std::vector<MeasuredData*> measuredData() const;

	Graph2dVerificationWindowResultSetting::GraphType graphType() const;
	int timeStep() const;
	v4PostZoneDataContainer* activePostData() const;
	QString activeResult() const;
	MeasuredData* activeMeasuredData() const;
	QString activeValue() const;
	QString activeFile() const;
	QString activeZone() const;

	void setActivePostData(v4PostZoneDataContainer* container);
	void setActiveMeasuredData(MeasuredData* activeMeasuredData);
	void setActiveValue(QString activeValue);
	void setActiveResult(QString activeResult);
	void setGraphType(GraphType graphType);
	void setTimeStep(int timestep);
	void setActiveFile(QString activeFile);
	void setActiveZone(QString activeZone);
	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	void updateActivePostData();

private:

	//friend class Graph2dVerificationWindowDataModel;

	PostSolutionInfo* m_postSolutionInfo;
	std::vector<MeasuredData*> m_measuredData;

	v4PostZoneDataContainer* m_activePostData;
	QString m_activeResult;

	MeasuredData* m_activeMeasuredData;
	GraphType m_graphType;

	int m_timeStep;
	QString m_activeValue;

	QString m_activeFile;
	QString m_activeZone;
};

#endif // GRAPH2DVERIFICATIONWINDOWRESULTSETTING_H
