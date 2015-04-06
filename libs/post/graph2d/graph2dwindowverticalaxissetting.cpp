#include "graph2dwindowverticalaxissetting.h"

Graph2dWindowVerticalAxisSetting::Graph2dWindowVerticalAxisSetting()
	: ProjectDataItem(0)
{
	init();
}

Graph2dWindowVerticalAxisSetting::Graph2dWindowVerticalAxisSetting(ProjectDataItem* p)
	: ProjectDataItem(p)
{
	init();
}

void Graph2dWindowVerticalAxisSetting::init()
{
	m_rangeMode = rmAuto;
	m_labelMode = lmAuto;
	m_customRangeMin = 0;
	m_customRangeMax = 0;
}

void Graph2dWindowVerticalAxisSetting::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Graph2dWindowVerticalAxisSetting::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

Graph2dWindowVerticalAxisSetting& Graph2dWindowVerticalAxisSetting::operator =(const Graph2dWindowVerticalAxisSetting& setting)
{
	m_rangeMode = setting.m_rangeMode;
	m_customRangeMin = setting.m_customRangeMin;
	m_customRangeMax = setting.m_customRangeMax;
	m_labelMode = setting.m_labelMode;
	m_customLabel = setting.m_customLabel;
	return *this;
}
