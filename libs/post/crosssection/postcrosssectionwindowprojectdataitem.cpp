#include "postcrosssectioninternalwindow.h"
#include "postcrosssectionwindow.h"
#include "postcrosssectionwindowprojectdataitem.h"

#include <guicore/post/postprocessorwindow.h>

PostCrosssectionWindowProjectDataItem::PostCrosssectionWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow) :
	PostProcessorWindowProjectDataItem(parent, parentWindow),
	m_geometry {}
{
	auto w = new PostCrosssectionWindow(parentWindow, index, this);
	m_window = w;
}

bool PostCrosssectionWindowProjectDataItem::setupInitialSetting()
{
	auto w = dynamic_cast<PostCrosssectionWindow*> (m_window);
	auto iw = w->internalWindow();
	return iw->setupInitialSetting();
}

QString PostCrosssectionWindowProjectDataItem::windowId() const
{
	return QString("postcrosssectionwindow%1").arg(m_window->index());
}

void PostCrosssectionWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto w = dynamic_cast<PostCrosssectionWindow*> (m_window);
	w->internalWindow()->loadFromProjectMainFile(node);
}

void PostCrosssectionWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	auto w = dynamic_cast<PostCrosssectionWindow*> (m_window);
	w->internalWindow()->saveToProjectMainFile(writer);
}
