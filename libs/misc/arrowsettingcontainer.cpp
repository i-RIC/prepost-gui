#include "arrowsettingcontainer.h"
#include "xmlsupport.h"
#include <QDomNode>
#include <QXmlStreamWriter>

const int ArrowSettingContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowSettingContainer::DEFAULT_LINEWIDTH = 1;

ArrowSettingContainer::ArrowSettingContainer()
{
	m_arrowSize = DEFAULT_ARROWSIZE;
	m_lineWidth = DEFAULT_LINEWIDTH;
}

void ArrowSettingContainer::load(const QDomNode &node)
{
	m_arrowSize = iRIC::getIntAttribute(node, attName("arrowSize"), DEFAULT_ARROWSIZE);
	m_lineWidth = iRIC::getIntAttribute(node, attName("arrowLineWidth"), DEFAULT_LINEWIDTH);
}

void ArrowSettingContainer::save(QXmlStreamWriter &writer) const
{
	iRIC::setIntAttribute(writer, attName("arrowSize"), m_arrowSize);
	iRIC::setIntAttribute(writer, attName("arrowLineWidth"), m_lineWidth);
}
