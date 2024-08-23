#define _USE_MATH_DEFINES  1  // set to 1 to match qwt_math.h
#include <cmath>

#include "graph2dverificationwindowresultsetting.h"

#include <guicore/project/measured/measureddata.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Graph2dVerificationWindowResultSetting::Graph2dVerificationWindowResultSetting()
{
	m_postSolutionInfo = nullptr;
	m_timeStep = 0;
	m_graphType = Graph2dVerificationWindowResultSetting::gtSWDvsValues;
	m_activePostData = nullptr;
	m_activeMeasuredData = nullptr;
}

Graph2dVerificationWindowResultSetting::~Graph2dVerificationWindowResultSetting()
{
}

bool Graph2dVerificationWindowResultSetting::init(PostSolutionInfo* sol, const std::vector<MeasuredData*>& measuredData)
{
	m_measuredData = measuredData;
	m_postSolutionInfo = sol;
	m_timeStep = m_postSolutionInfo->currentStep();

	if (m_postSolutionInfo == nullptr) {
		return false;
	}
	return true;
}

Graph2dVerificationWindowResultSetting& Graph2dVerificationWindowResultSetting::operator=(const Graph2dVerificationWindowResultSetting& s)
{
	m_postSolutionInfo = s.m_postSolutionInfo;
	m_measuredData = s.m_measuredData;

	m_activePostData = s.m_activePostData;
	m_activeResult = s.m_activeResult;

	m_activeMeasuredData = s.m_activeMeasuredData;
	m_graphType = s.m_graphType;

	m_timeStep = s.m_timeStep;
	m_activeValue = s.m_activeValue;

	m_activeFile = s.m_activeFile;
	m_activeZone = s.m_activeZone;

	return *this;
}

PostSolutionInfo* Graph2dVerificationWindowResultSetting::postSolutionInfo() const
{
	return m_postSolutionInfo;
}

std::vector<MeasuredData*> Graph2dVerificationWindowResultSetting::measuredData() const
{
	return m_measuredData;
}

Graph2dVerificationWindowResultSetting::GraphType Graph2dVerificationWindowResultSetting::graphType() const
{
	return m_graphType;
}

int Graph2dVerificationWindowResultSetting::timeStep() const
{
	return m_timeStep;
}

v4PostZoneDataContainer* Graph2dVerificationWindowResultSetting::activePostData() const
{
	return m_activePostData;
}

QString Graph2dVerificationWindowResultSetting::activeResult() const
{
	return m_activeResult;
}

MeasuredData* Graph2dVerificationWindowResultSetting::activeMeasuredData() const
{
	return m_activeMeasuredData;
}

QString Graph2dVerificationWindowResultSetting::activeValue() const
{
	return m_activeValue;
}

QString Graph2dVerificationWindowResultSetting::activeFile() const
{
	return m_activeFile;
}

QString Graph2dVerificationWindowResultSetting::activeZone() const
{
	return m_activeZone;
}

void Graph2dVerificationWindowResultSetting::loadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	m_graphType = static_cast<GraphType>(iRIC::getIntAttribute(node, "graphType"));
	m_activeResult = elem.attribute("activeResult");
	m_activeValue = elem.attribute("activeValue");
	m_activeFile = elem.attribute("activeFile");
	m_activeZone = elem.attribute("activeZone");

	for (MeasuredData* md : m_measuredData) {
		if (m_activeFile == md->name()) {
			m_activeMeasuredData = md;
			break;
		}
	}

	updateActivePostData();
}

void Graph2dVerificationWindowResultSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setIntAttribute(writer, "graphType", static_cast<int>(m_graphType));
	writer.writeAttribute("activeResult", m_activeResult);
	writer.writeAttribute("activeValue", m_activeValue);
	writer.writeAttribute("activeFile", m_activeFile);
	writer.writeAttribute("activeZone", m_activeZone);
}

void Graph2dVerificationWindowResultSetting::updateActivePostData()
{
	m_activePostData = nullptr;
	for (auto cont : m_postSolutionInfo->v4ZoneContainers2D()) {
		if (m_activeZone == cont->zoneName().c_str()) {
			m_activePostData = cont;
			break;
		}
	}
}

void Graph2dVerificationWindowResultSetting::setActivePostData(v4PostZoneDataContainer* postZoneDataContainer)
{
	m_activePostData = postZoneDataContainer;
}

void Graph2dVerificationWindowResultSetting::setActiveMeasuredData(MeasuredData* activeMeasuredData)
{
	m_activeMeasuredData = activeMeasuredData;
}

void Graph2dVerificationWindowResultSetting::setActiveValue(QString activeValue)
{
	m_activeValue = activeValue;
}

void Graph2dVerificationWindowResultSetting::setActiveResult(QString activeResult)
{
	m_activeResult = activeResult;
}

void Graph2dVerificationWindowResultSetting::setGraphType(GraphType graphType)
{
	m_graphType = graphType;
}

void Graph2dVerificationWindowResultSetting::setTimeStep(int timestep)
{
	m_timeStep = timestep;
}

void Graph2dVerificationWindowResultSetting::setActiveFile(QString activeFile)
{
	m_activeFile = activeFile;
}

void Graph2dVerificationWindowResultSetting::setActiveZone(QString activeZone)
{
	m_activeZone = activeZone;
}
