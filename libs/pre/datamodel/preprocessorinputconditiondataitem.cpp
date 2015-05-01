#include "preprocessorinputconditiondataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/project/inputcond/cgnsfileinputconditiondialog.h>
#include <guicore/project/inputcond/cgnsfileinputconditionwidgetfilename.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QLocale>
#include <QMenu>
#include <QStandardItem>

#include <cgnslib.h>

PreProcessorInputConditionDataItem::PreProcessorInputConditionDataItem(GraphicsWindowDataItem* parent)
	: PreProcessorDataItem(parent)
{
	try {
		iRICMainWindowInterface* mainW = projectData()->mainWindow();
		m_dialog = new CgnsFileInputConditionDialog(projectData()->solverDefinition(), mainW->locale() , mainW);
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
	if (m_dialog) {
		delete m_dialog;
	}
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

void PreProcessorInputConditionDataItem::loadFromCgnsFile(const int fn)
{
	m_dialog->load(fn);
}

void PreProcessorInputConditionDataItem::saveToCgnsFile(const int fn)
{
	m_dialog->save(fn);
}

void PreProcessorInputConditionDataItem::showDialog(bool readonly)
{
	projectData()->mainfile()->postSolutionInfo()->close();
	QString fname = projectData()->currentCgnsFileName();
	int fn;
	cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &fn);
	// load data.
	loadFromCgnsFile(fn);
	cg_close(fn);
	// set default folder for filename input conditions.
	CgnsFileInputConditionWidgetFilename::defaultFolder = LastIODirectory::get();
	m_dialog->setFileName(fname);
	// show dialog
	m_dialog->setReadOnly(readonly);
	m_dialog->exec();
	// set the default folder back.
	LastIODirectory::set(CgnsFileInputConditionWidgetFilename::defaultFolder);
	m_isSet = true;
}

bool PreProcessorInputConditionDataItem::importInputCondition(const QString& filename)
{
	projectData()->mainfile()->postSolutionInfo()->close();
	QString fname = projectData()->currentCgnsFileName();
	m_dialog->setFileName(fname);
	bool ret = m_dialog->import(filename);
	if (ret) {m_isSet = true;}
	return ret;
}

bool PreProcessorInputConditionDataItem::exportInputCondition(const QString& filename)
{
	QString fname = projectData()->currentCgnsFileName();
	m_dialog->setFileName(fname);
	return m_dialog->doExport(filename);
}
