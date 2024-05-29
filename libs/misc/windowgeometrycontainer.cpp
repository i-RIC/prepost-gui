#include "windowgeometrycontainer.h"
#include "xmlsupport.h"

#include <QDomNode>
#include <QWidget>
#include <QXmlStreamWriter>

WindowGeometryContainer::WindowGeometryContainer() :
	WindowGeometryContainer(nullptr)
{}

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

	bool min = iRIC::getBooleanAttribute(node, "minimized", false);
	bool max = iRIC::getBooleanAttribute(node, "maximized", false);
	if (min) {
		m_window->showMinimized();
	} else if (max) {
		m_window->showMaximized();
	}
}

void WindowGeometryContainer::save(QXmlStreamWriter& writer) const
{
	bool max = m_window->isMaximized();
	bool min = m_window->isMinimized();

	iRIC::setBooleanAttribute(writer, "minimized", min);
	iRIC::setBooleanAttribute(writer, "maximized", max);

	if (max || min) {
		m_window->setWindowState(Qt::WindowNoState);
	}

	QRect rect = m_window->frameGeometry();
	iRIC::setIntAttribute(writer, "left", rect.left());
	iRIC::setIntAttribute(writer, "top", rect.top());
	iRIC::setIntAttribute(writer, "width", rect.width());
	iRIC::setIntAttribute(writer, "height", rect.height());

	if (min) {
		m_window->showMinimized();
	} else if (max) {
		m_window->showMaximized();
	}
}

void WindowGeometryContainer::setWidget(QWidget* w)
{
	m_window = w;
}

void WindowGeometryContainer::setIgnoreMax(bool ignoreMax)
{
	m_ignoreMax = ignoreMax;
}
