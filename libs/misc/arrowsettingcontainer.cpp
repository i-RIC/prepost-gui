#include "arrowsettingcontainer.h"
#include "xmlsupport.h"

#include <QDomNode>
#include <QXmlStreamWriter>

const int ArrowSettingContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowSettingContainer::DEFAULT_LINEWIDTH = 1;

ArrowSettingContainer::ArrowSettingContainer() :
	CompositeContainer {&m_arrowSize, &m_lineWidth},
	m_arrowSize {"arrowSize", DEFAULT_ARROWSIZE},
	m_lineWidth {"arrowLineWidth", DEFAULT_LINEWIDTH}
{}

ArrowSettingContainer::ArrowSettingContainer(const ArrowSettingContainer& c) :
	ArrowSettingContainer {}
{
	CompositeContainer::operator=(c);
}
