#include "windowgeometrycontainer.h"
#include "xmlsupport.h"

#include <QDomNode>
#include <QWidget>
#include <QXmlStreamWriter>

WindowGeometryContainer::WindowGeometryContainer(QWidget* w, bool ignoreMax) :
	m_ignoreMax {ignoreMax},
	m_window {w}
{}

void WindowGeometryContainer::load(const QDomNode& node)
{
	QPoint pos;
	pos.setX(iRIC::getIntAttribute(node, "left", 0));
	pos.setY(iRIC::getIntAttribute(node, "top", 0));
	m_window->move(pos);

	QSize size;
	size.setWidth(iRIC::getIntAttribute(node, "width", 640));
	size.setHeight(iRIC::getIntAttribute(node, "height", 480));
	m_window->resize(size);

	if (! m_ignoreMax && iRIC::getBooleanAttribute(node, "maximized")) {
		m_window->showMaximized();
	}
	m_window->setVisible(iRIC::getBooleanAttribute(node, "visible"));
}

void WindowGeometryContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setBooleanAttribute(writer, "visible", m_window->isVisible());
	iRIC::setBooleanAttribute(writer, "maximized", m_window->isMaximized());

	QRect rect = m_window->frameGeometry();
	iRIC::setIntAttribute(writer, "left", rect.left());
	iRIC::setIntAttribute(writer, "top", rect.top());
	iRIC::setIntAttribute(writer, "width", rect.width());
	iRIC::setIntAttribute(writer, "height", rect.height());
}
