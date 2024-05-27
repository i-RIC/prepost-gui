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

	m_window->setWindowState(static_cast<Qt::WindowStates>(iRIC::getIntAttribute(node, "state", 0)));
}

void WindowGeometryContainer::save(QXmlStreamWriter& writer) const
{
	auto state = (m_window->windowState() & 7);
	iRIC::setIntAttribute(writer, "state", static_cast<int> (state));
	if (state != 0) {
		m_window->setWindowState(Qt::WindowNoState);
	}

	QRect rect = m_window->frameGeometry();
	iRIC::setIntAttribute(writer, "left", rect.left());
	iRIC::setIntAttribute(writer, "top", rect.top());
	iRIC::setIntAttribute(writer, "width", rect.width());
	iRIC::setIntAttribute(writer, "height", rect.height());

	m_window->setWindowState(state);
}

void WindowGeometryContainer::setWidget(QWidget* w)
{
	m_window = w;
}

void WindowGeometryContainer::setIgnoreMax(bool ignoreMax)
{
	m_ignoreMax = ignoreMax;
}
