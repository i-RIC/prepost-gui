#include "solverconsolewindow.h"
#include "solverconsolewindowprojectdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <misc/lastiodirectory.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QTextStream>
#include <QXmlStreamWriter>

SolverConsoleWindowProjectDataItem::SolverConsoleWindowProjectDataItem(SolverConsoleWindow* w, ProjectDataItem* parent) :
	ProjectDataItem(parent),
	m_solverConsoleWindow {w},
	m_geometry {w->parentWidget(), true}
{
	setFilename("consoleLog.txt");
	m_file.setFileName(filename());

	clear();
	m_solverConsoleWindow->setBackgroundColor(QColor(200, 200, 200));
}

SolverConsoleWindowProjectDataItem::~SolverConsoleWindowProjectDataItem()
{
	m_solverConsoleWindow->exportLogAction->setDisabled(true);
}

void SolverConsoleWindowProjectDataItem::initForSolverDefinition()
{
	m_solverConsoleWindow->m_projectData = projectData();
	m_solverConsoleWindow->updateWindowTitle();
}

void SolverConsoleWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	// load window geometry
	m_geometry.load(node);
	// load the log of the last run.
	QDomNode consoleLog = iRIC::getChildNode(node, "Consolelog");
	QString logText = iRIC::getText(consoleLog);
	QString fname = filename();
	QFileInfo finfo(fname);
	if (finfo.size() == 0) {
		// write the log.
		QFile f(fname);
		f.open(QFile::WriteOnly | QFile::Text);
		QTextStream ts(&f);
		ts << logText;
		f.close();
	}
	// load background color
	QColor col = ProjectDataItem::loadBackgroundColor(node, QColor(200, 200, 200));
	m_solverConsoleWindow->setBackgroundColor(col);
}

void SolverConsoleWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// save window geometry
	m_geometry.save(writer);
	// save background color
	ProjectDataItem::writeBackgroundColor(m_solverConsoleWindow->backgroundColor(), writer);
}

void SolverConsoleWindowProjectDataItem::clear()
{
	QString fname = filename();
	QFile f(fname);
	// open, and write nothing.
	f.open(QFile::WriteOnly);
	f.close();
}

void SolverConsoleWindowProjectDataItem::append(const QString& line)
{
	QTextStream ts(&m_file);
	ts << line << endl;

	appendToLines(line);
	m_solverConsoleWindow->m_console->appendPlainText(line);
}

void SolverConsoleWindowProjectDataItem::appendToLines(const QString& line)
{
	m_lines.append(line);
	while (m_lines.count() > MAXLINES) {
		m_lines.pop_front();
	}
}

void SolverConsoleWindowProjectDataItem::loadExternalData(const QString& filename)
{
	QFile f(filename);
	// open, and write nothing.
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream ts(&f);
	while (! ts.atEnd()) {
		QString line = ts.readLine();
		appendToLines(line);
	}
	f.close();
	QString log = m_lines.join("\n");
	m_solverConsoleWindow->m_console->setPlainText(log);
	m_solverConsoleWindow->m_console->moveCursor(QTextCursor::End);

	QFileInfo finfo(f);
	if (finfo.size() != 0) {
		// enable export action.
		m_solverConsoleWindow->exportLogAction->setEnabled(true);
	}
}

void SolverConsoleWindowProjectDataItem::open()
{
	m_file.open(QFile::Append | QFile::Text);
	setModified();
}

void SolverConsoleWindowProjectDataItem::close()
{
	m_file.close();
}

void SolverConsoleWindowProjectDataItem::exportConsoleLog()
{
	QString defName =  QDir(LastIODirectory::get()).absoluteFilePath("consolelog.txt");
	QString fname = QFileDialog::getSaveFileName(
										projectData()->mainWindow(), tr("Select File to Export"), defName, tr("Text file (*.txt)")
									);
	if (fname == "") {return;}
	projectData()->mainWindow()->statusBar()->showMessage(tr("Exporting solver console log..."));
	bool ret = true;
	if (QFile::exists(fname)) {
		// if the file already existed, remove it first.
		ret = QFile::remove(fname);
	}
	ret = ret && QFile::copy(filename(), fname);
	if (ret) {
		projectData()->mainWindow()->statusBar()->showMessage(tr("Solver console log is successfully exported to %1.").arg(fname), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} else {
		projectData()->mainWindow()->statusBar()->clearMessage();
		QMessageBox::critical(projectData()->mainWindow(), tr("Fail"), tr("Exporting solver console log failed."));
	}
}
