#include "preprocessorinputconditiondataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/inputcond/inputconditiondialog.h>
#include <guicore/project/inputcond/inputconditionwidgetfilename.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QFileInfo>
#include <QLocale>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <cgnslib.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>

PreProcessorInputConditionDataItem::PreProcessorInputConditionDataItem(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem {parent}
{
	try {
		iRICMainWindowInterface* mainW = projectData()->mainWindow();
		m_dialog = new InputConditionDialog(projectData()->solverDefinition(), mainW->locale() , mainW);
		m_dialog->setWorkFolder(projectData()->workDirectory());
		connect(m_dialog, SIGNAL(accepted()), this, SLOT(setModified()));
		m_isDeletable = false;
		m_isSet = false;
	} catch (ErrorMessage&) {
		m_dialog = nullptr;
	}
}

PreProcessorInputConditionDataItem::~PreProcessorInputConditionDataItem()
{
	delete m_dialog;
}

void PreProcessorInputConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_isSet = (node.toElement().attribute("isSet") == "true");
}

void PreProcessorInputConditionDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString isSetStr;
	if (m_isSet) {isSetStr = "true";} else {isSetStr = "false";}
	writer.writeAttribute("isSet", isSetStr);
}

int PreProcessorInputConditionDataItem::loadFromCgnsFile()
{
	auto cgnsFile = projectData()->mainfile()->cgnsFile();
	return m_dialog->load(*(cgnsFile->ccBase()->ccGroup()));
}

int PreProcessorInputConditionDataItem::saveToCgnsFile()
{
	auto cgnsFile = projectData()->mainfile()->cgnsFile();
	return m_dialog->save(cgnsFile->ccBase()->ccGroup());
}

int PreProcessorInputConditionDataItem::updateCgnsFileOtherThanGrids()
{
	auto cgnsFile = projectData()->mainfile()->cgnsFile();
	return m_dialog->save(cgnsFile->ccBase()->ccGroup());
}

void PreProcessorInputConditionDataItem::showDialog(bool readonly)
{
	projectData()->mainfile()->postSolutionInfo()->close();

	// set default folder for filename input conditions.
	InputConditionWidgetFilename::defaultFolder = LastIODirectory::get();
	auto fname = projectData()->currentCgnsFileName();
	m_dialog->setFileName(fname);
	// show dialog
	m_dialog->setReadOnly(readonly);
	m_dialog->exec();
	// set the default folder back.
	LastIODirectory::set(InputConditionWidgetFilename::defaultFolder);
	m_isSet = true;
}

ProjectData* PreProcessorInputConditionDataItem::projectData() const
{
	return dynamic_cast<ProjectData*>(ProjectDataItem::projectData());
}

void PreProcessorInputConditionDataItem::handleStandardItemDoubleClicked()
{
	showDialog();
}

void PreProcessorInputConditionDataItem::checkImportSourceUpdate()
{
	m_dialog->checkImportSourceUpdate();
}

bool PreProcessorInputConditionDataItem::importInputCondition(const QString& filename)
{
	projectData()->mainfile()->postSolutionInfo()->close();
	QString fname = projectData()->currentCgnsFileName();
	m_dialog->setFileName(fname);

	bool ret;
	QFileInfo finfo(filename);
	if (finfo.suffix() == "yml") {
		ret = m_dialog->importFromYaml(filename);
	} else {
		ret = m_dialog->importFromCgns(filename);
	}
	if (ret) {m_isSet = true;}
	return ret;
}

bool PreProcessorInputConditionDataItem::exportInputCondition(const QString& filename)
{
	QString fname = projectData()->currentCgnsFileName();
	m_dialog->setFileName(fname);

	QFileInfo finfo(filename);
	if (finfo.suffix() == "yml") {
		return m_dialog->exportToYaml(filename);
	} else {
		return m_dialog->exportToCgns(filename);
	}
}

bool PreProcessorInputConditionDataItem::setupCgnsFilesIfNeeded(QString *cgnsFileForGrid, bool* updated)
{
	return m_dialog->setupCgnsFilesIfNeeded(cgnsFileForGrid, updated);
}

bool PreProcessorInputConditionDataItem::isSet() const
{
	return m_isSet;
}

bool PreProcessorInputConditionDataItem::isSetupCorrectly() const
{
	return m_dialog != nullptr;
}

void PreProcessorInputConditionDataItem::setModified(bool modified)
{
	PreProcessorDataItem::setModified(modified);
}
