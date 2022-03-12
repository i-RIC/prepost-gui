#include "../base/iricmainwindowinterface.h"
#include "../post/postprocessorwindow.h"
#include "../post/postprocessorwindowfactoryinterface.h"
#include "../post/postprocessorwindowfactoryinterface.h"
#include "../post/postprocessorwindowprojectdataitem.h"
#include "projectdata.h"
#include "projectpostprocessors.h"
//#include "main/iricmainwindow.h"
//#include "post/postprocessorwindow.h"
//#include "post/postprocessorwindowfactory.h"
//#include "post/postprocessorwindowprojectdataitem.h"
//#include "projectdata.h"

#include <misc/errormessage.h>

#include <QDir>
#include <QDomNode>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>
#include <QXmlStreamWriter>

namespace {
	static QString SUBWINDOWS = "subwindows";
}

const QString ProjectPostProcessors::subwindowsFolder()
{
	return SUBWINDOWS;
}

ProjectPostProcessors::ProjectPostProcessors(ProjectDataItem* parent) :
	ProjectDataItem(parent)
{}

ProjectPostProcessors::~ProjectPostProcessors()
{
	if (projectData()->mainWindow() == nullptr) {return;}
	// remove all post processors from the MDI area of main window.
	QMdiArea* area = dynamic_cast<QMdiArea*>(projectData()->mainWindow()->centralWidget());

	for (auto it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it) {
		area->removeSubWindow((*it)->window()->parentWidget());
	}
}

void ProjectPostProcessors::loadFromXmlFile(const QDomNode& node, const QDir workDir)
{
	auto nodeName = node.nodeName();
	if (nodeName == "iRICPostProcessingSettings" || nodeName == "PostProcessors") {
		QDomNode child = node.firstChild();
		while (! child.isNull()) {
			loadSingleWindowFromXmlFile(child, workDir);
			child = child.nextSibling();
		}
	} else if (nodeName == "PostProcessor") {
		loadSingleWindowFromXmlFile(node, workDir);
	}
}

void ProjectPostProcessors::loadSingleWindowFromXmlFile(const QDomNode& node, const QDir workDir)
{
	QWidget* parentWindow = projectData()->mainWindow();

	PostProcessorWindowProjectDataItem* di = nullptr;
	QDomNode windowNode;
	QDomElement childElem = node.toElement();

	auto ref = childElem.attribute("ref");
	if (ref != "") {
		// file is saved in separate XML file
		QString fname = workDir.filePath(ref);
		if (QFile::exists(fname)) {
			QFile f(fname);
			QDomDocument doc;
			bool ok = doc.setContent(&f);
			if (ok) {
				di = m_factory->restore(doc.documentElement(), this, parentWindow);
				windowNode = doc.documentElement();
			}
		}
	} else {
		di = m_factory->restore(node, this, parentWindow);
		windowNode = node;
	}

	if (di != nullptr) {
		add(di);
		try {
			di->loadFromProjectMainFile(windowNode);
		} catch (ErrorMessage m) {
			m_postProcessorWindows.removeOne(di);
			delete di;
		}
	}
}

void ProjectPostProcessors::saveToXmlFile(QXmlStreamWriter& writer, const QDir workDir)
{
	// Create subwindows folder
	if (! workDir.exists(SUBWINDOWS)) {
		workDir.mkdir(SUBWINDOWS);
	}
	QDir subWindowsDir(workDir.filePath(SUBWINDOWS));

	for (auto w : m_postProcessorWindows) {
		auto fileName = subWindowsDir.filePath(QString("%1.xml").arg(w->windowId()));

		// Write reference
		writer.writeStartElement("PostProcessor");
		writer.writeAttribute("ref", QString("%1/%2.xml").arg(SUBWINDOWS).arg(w->windowId()));
		writer.writeEndElement();

		QFile f(fileName);
		f.open(QFile::WriteOnly);
		QXmlStreamWriter writer2(&f);
		writer2.setAutoFormatting(true);
		writer2.writeStartDocument("1.0");
		writer2.writeStartElement("PostProcessor");
		w->saveToProjectMainFile(writer2);
		writer2.writeEndElement();
		writer2.writeEndDocument();
		f.close();
	}
}

void ProjectPostProcessors::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDir workDir(projectData()->workDirectory());

	loadFromXmlFile(node, workDir);
}

void ProjectPostProcessors::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QDir workDir(projectData()->workDirectory());

	saveToXmlFile(writer, workDir);
}

QMdiSubWindow* ProjectPostProcessors::add(PostProcessorWindowProjectDataItem* newitem)
{
	iRICMainWindowInterface* w = projectData()->mainWindow();
	if (w == nullptr) {return nullptr;}
	QMdiArea* area = dynamic_cast<QMdiArea*>(w->centralWidget());
	QMdiSubWindow* container = area->addSubWindow(newitem->window());
	container->setWindowIcon(newitem->window()->icon());

	m_postProcessorWindows.push_back(newitem);
	return container;
}

void ProjectPostProcessors::requestDelete(PostProcessorWindowProjectDataItem* item)
{
	for (auto it = m_postProcessorWindows.begin(); it != m_postProcessorWindows.end(); ++it) {
		if (*it == item) {
			m_postProcessorWindows.erase(it);
			delete item;
			return;
		}
	}
}

int ProjectPostProcessors::windowCount() const
{
	return m_postProcessorWindows.count();
}

void ProjectPostProcessors::setFactory(PostProcessorWindowFactoryInterface* factory)
{
	m_factory = factory;
}

void ProjectPostProcessors::applyOffset(double x_diff, double y_diff)
{
	for (auto w : m_postProcessorWindows) {
		w->window()->applyOffset(x_diff, y_diff);
	}
}

QStringList ProjectPostProcessors::containedFiles() const
{
	QStringList ret;

	for (auto w : m_postProcessorWindows) {
		ret.append(QString("%1/%2.xml").arg(SUBWINDOWS).arg(w->windowId()));
	}

	return ret;
}

