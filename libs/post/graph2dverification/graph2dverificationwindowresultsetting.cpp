#define _USE_MATH_DEFINES  1  // set to 1 to match qwt_math.h
#include <cmath>

#include "graph2dverificationwindowresultsetting.h"

#include <guicore/project/measured/measureddata.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
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

bool Graph2dVerificationWindowResultSetting::init(PostSolutionInfo* sol, const std::vector<MeasuredData*>& measuredData, const QString& /* cgnsFilename */)
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
	for (PostZoneDataContainer* cont : m_postSolutionInfo->zoneContainers2D()) {
		if (m_activeZone == cont->zoneName().c_str()) {
			m_activePostData = cont;
			break;
		}
	}
}

void Graph2dVerificationWindowResultSetting::setActivePostData(PostZoneDataContainer* postZoneDataContainer)
{
	m_activePostData = postZoneDataContainer;
}
