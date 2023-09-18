#include "../base/iricmainwindowi.h"
#include "../post/postprocessorwindow.h"
#include "../post/postprocessorwindowfactoryi.h"
#include "../post/postprocessorwindowfactoryi.h"
#include "../post/postprocessorwindowprojectdataitem.h"
#include "projectdata.h"
#include "projectpostprocessors.h"

#include <guibase/widget/itemmultiselectingdialog.h>
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
	auto area = dynamic_cast<QMdiArea*>(projectData()->mainWindow()->centralWidget());

	for (auto w : m_postProcessorWindows) {
		area->removeSubWindow(w->window()->parentWidget());
	}
}

void ProjectPostProcessors::loadFromXmlFile(const QDomNode& node, const QDir workDir)
{
	auto nodeName = node.nodeName();
	if (nodeName == "iRICPostProcessingSettings" || nodeName == "PostProcessors") {
		QDomNode child = node.firstChild();
		while (! child.isNull()) {
			loadSingleWindowFromXmlFile(child, workDir, false);
			child = child.nextSibling();
		}
	} else if (nodeName == "PostProcessor") {
		loadSingleWindowFromXmlFile(node, workDir, false);
	}
}

void ProjectPostProcessors::loadSingleWindowFromXmlFile(const QDomNode& node, const QDir workDir, bool ignoreIndex)
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
			auto index = di->window()->index();
			di->loadFromProjectMainFile(windowNode);
			if (ignoreIndex) {
				di->window()->setIndex(index);
			}
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

bool ProjectPostProcessors::importFromXmlFile(const QDomNode& node, const QDir& workDir, QWidget* w)
{
	QWidget* parentWindow = projectData()->mainWindow();

	std::vector<QString> items;
	std::vector<QDomNode> nodes;
	const auto& children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		auto child = children.at(i).toElement();
		PostProcessorWindowProjectDataItem* di = nullptr;
		auto ref = child.attribute("ref");
		if (ref != "") {
			// separate XML file
			QString fileName = workDir.filePath(ref);
			if (! QFile::exists(fileName)) {continue;}

			QFile f(fileName);
			QDomDocument doc;
			bool ok = doc.setContent(&f);
			if (! ok) {continue;}

			di = m_factory->restore(doc.documentElement(), this, nullptr);
		} else {
			di = m_factory->restore(child, this, nullptr);
		}
		items.push_back(di->window()->windowTitle());
		nodes.push_back(child);
		delete di;
	}

	ItemMultiSelectingDialog dialog(parentWindow);
	dialog.setItems(items);
	dialog.checkAll();
	dialog.setWindowTitle(tr("Select import target windows"));

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}
	auto selectSettings = dialog.selectSettings();

	for (int i = 0; i < selectSettings.size(); ++i) {
		bool selected = selectSettings.at(i);
		if (! selected) {continue;}

		auto node = nodes.at(i);
		loadSingleWindowFromXmlFile(node, workDir, true);
	}

	return true;
}

bool ProjectPostProcessors::exportToSingleXmlFile(QXmlStreamWriter& writer, QWidget* w)
{
	std::vector<QString> items;
	for (auto w : m_postProcessorWindows) {
		items.push_back(w->window()->windowTitle());
	}

	ItemMultiSelectingDialog dialog(w);
	dialog.setItems(items);
	dialog.checkAll();
	dialog.setWindowTitle(tr("Select export target windows"));

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}
	auto selectSettings = dialog.selectSettings();

	for (int i = 0; i < m_postProcessorWindows.size(); ++i) {
		bool selected = selectSettings.at(i);
		if (! selected) {continue;}

		auto w = m_postProcessorWindows.at(i);
		writer.writeStartElement("PostProcessor");
		w->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	return true;
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
	iRICMainWindowI* w = projectData()->mainWindow();
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

void ProjectPostProcessors::setFactory(PostProcessorWindowFactoryI* factory)
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

