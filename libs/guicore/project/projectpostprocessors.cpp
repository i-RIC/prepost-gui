#include "projectpostprocessors.h"
//#include "projectdata.h"
//#include "post/postprocessorwindowfactory.h"
//#include "main/iricmainwindow.h"
//#include "post/postprocessorwindowprojectdataitem.h"
//#include "post/postprocessorwindow.h"
#include "../post/postprocessorwindow.h"
#include "../post/postprocessorwindowprojectdataitem.h"
#include "../post/postprocessorwindowfactoryinterface.h"
#include "projectdata.h"
#include "../base/iricmainwindowinterface.h"
#include <misc/errormessage.h>
#include "../post/postprocessorwindowfactoryinterface.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMdiArea>
#include <QMdiSubWindow>

ProjectPostProcessors::ProjectPostProcessors(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{

}

ProjectPostProcessors::~ProjectPostProcessors()
{
	if (projectData()->mainWindow() == 0){return;}
	// remove all post processors from the MDI area of main window.
	QMdiArea* area = dynamic_cast<QMdiArea*> (projectData()->mainWindow()->centralWidget());

	QList<PostProcessorWindowProjectDataItem*>::iterator it;
	for (it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it){
		area->removeSubWindow((*it)->window()->parentWidget());
	}
}

void ProjectPostProcessors::doLoadFromProjectMainFile(const QDomNode& node, bool import)
{
	QDomNode child = node.firstChild();
	QWidget* parentWindow = projectData()->mainWindow();
	while (! child.isNull()){
		PostProcessorWindowProjectDataItem* di = 0;
		if (import){
			QDomElement elem = child.toElement();
			di = m_factory->factory(elem.attribute("type"), this, parentWindow);
		} else {
			di = m_factory->restore(child, this, parentWindow);
		}
		if (di != 0){
			add(di);
			try {
				di->loadFromProjectMainFile(child);
			} catch (ErrorMessage m){
				m_postProcessorWindows.removeOne(di);
				delete di;
			}
		}
		child = child.nextSibling();
	}
}

void ProjectPostProcessors::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QList<PostProcessorWindowProjectDataItem*>::iterator it;
	for (it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it){
		writer.writeStartElement("PostProcessor");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QMdiSubWindow* ProjectPostProcessors::add(PostProcessorWindowProjectDataItem* newitem)
{
	iRICMainWindowInterface* w = projectData()->mainWindow();
	if (w == 0){return 0;}
	QMdiArea* area = dynamic_cast<QMdiArea*> (w->centralWidget());
	QMdiSubWindow* container = area->addSubWindow(newitem->window());
	container->setWindowIcon(newitem->window()->icon());

	m_postProcessorWindows.push_back(newitem);
	return container;
}

void ProjectPostProcessors::requestDelete(PostProcessorWindowProjectDataItem* item)
{
	QList<PostProcessorWindowProjectDataItem*>::iterator it;
	for (it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it){
		if (*it == item){
			m_postProcessorWindows.erase(it);
			delete item;
			return;
		}
	}
}

void ProjectPostProcessors::loadFromProjectMainFile(const QDomNode& node, bool import)
{
	doLoadFromProjectMainFile(node, import);
}

int ProjectPostProcessors::windowCount() const
{
	return m_postProcessorWindows.count();
}

void ProjectPostProcessors::setFactory(PostProcessorWindowFactoryInterface* factory)
{
	m_factory = factory;
}
