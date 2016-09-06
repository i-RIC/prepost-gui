#include "solverconsolewindow.h"
#include "solverconsolewindowprojectdataitem.h"
#include "private/solverconsolewindow_impl.h"
#include "private/solverconsolewindow_setbackgroundcolorcommand.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/cgnsfileentry.h>
#include <guicore/project/cgnsfilelist.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QColorDialog>
#include <QFile>
#include <QFont>
#include <QIODevice>
#include <QLocale>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>

SolverConsoleWindow::Impl::Impl(iRICMainWindowInterface* mainW, SolverConsoleWindow* w) :
	m_iricMainWindow {mainW},
	m_process {nullptr},
	m_window {w}
{}

// public interfaces

SolverConsoleWindow::SolverConsoleWindow(iRICMainWindowInterface* parent) :
	QMainWindow {parent},
	impl {new Impl(parent, this)}
{
	init();
}

SolverConsoleWindow::~SolverConsoleWindow()
{
	delete impl;
}

void SolverConsoleWindow::init()
{
	setMinimumSize(480, 360);
	m_projectData = nullptr;
	m_destructing = false;

	exportLogAction = new QAction(tr("&Export solver console log..."), this);
	exportLogAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	exportLogAction->setDisabled(true);

	m_console = new QPlainTextEdit(this);
	m_console->setReadOnly(true);
	m_console->setAutoFillBackground(true);
	m_console->setWordWrapMode(QTextOption::WrapAnywhere);
	m_console->setMaximumBlockCount(SolverConsoleWindowProjectDataItem::MAXLINES);
	QFont font("Courier");
	font.setStyleHint(QFont::Courier);
	font.setPointSize(9);
	m_console->setFont(font);
	setCentralWidget(m_console);

	updateWindowTitle();
}

void SolverConsoleWindow::setProjectData(ProjectData* d)
{
	m_projectDataItem = new SolverConsoleWindowProjectDataItem(this, d->mainfile());
	connect(exportLogAction, SIGNAL(triggered()), m_projectDataItem, SLOT(exportConsoleLog()));
}

SolverConsoleWindowProjectDataItem* SolverConsoleWindow::projectDataItem()
{
	return m_projectDataItem;
}

bool SolverConsoleWindow::isSolverRunning()
{
	return impl->m_process != nullptr;
}

void SolverConsoleWindow::startSolver()
{
	// Check post only mode
	if (m_projectData->isPostOnlyMode()) {
		QMessageBox::information(this, tr("Information"), tr("This project is opened in post only mode. You can not run the solver."));
		return;
	}

	// Check grid is ready
	bool ok = m_projectData->mainWindow()->preProcessorWindow()->checkMappingStatus();
	if (! ok) {return;}

	// Check grid shape
	QSettings settings;
	QString msg = m_projectData->mainWindow()->preProcessorWindow()->checkGrid(settings.value("gridcheck/beforeexec", true).value<bool>());
	if (! msg.isEmpty()) {
		QString logFileName = m_projectData->absoluteFileName("gridcheck.txt");
		msg.append(QString("<a href=\"%1\">").arg(QString("file:///").append(logFileName))).append(tr("Show Detail")).append("</a>");
		int ret = QMessageBox::warning(m_projectData->mainWindow(), tr("Warning"), tr("The following problems found in the grid(s). Do you really want to run the solver with this grid?") + msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	// If the cgns file already has results, clear them first.
	if (m_projectData->mainfile()->hasResults() && QMessageBox::Cancel == QMessageBox::warning(this, tr("The simulation has result"), tr("Current simulation already has result data. When you run the solver, the current result data is discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
		return;
	}
	// discard result, and save now.
	try {
		m_projectData->mainfile()->clearResults();
	} catch (ErrorMessage& m) {
		QMessageBox::warning(this, tr("Warning"), tr("Error occured. %1").arg(m));
		return;
	}

	int ret = QMessageBox::information(this, tr("Information"), tr("We recommend that you save the project before starting the solver. Do you want to save?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
	if (ret == QMessageBox::Yes) {
		// save the project file.
		if (! m_projectData->mainWindow()->saveProject()) {return;}
	} else if (ret == QMessageBox::Cancel) {
		return;
	}
	clear();

	// check solver executable existance
	QString solver = m_projectData->solverDefinition()->executableFilename();
	QFile solverExec(solver);
	if (! solverExec.exists()) {
		// solver executable does not exists!!
		QMessageBox::critical(this, tr("Error"), tr("Solver executable file %1 does not exists.").arg(solver));
		return;
	}

	m_projectDataItem->open();

	startSolverSilently();

	updateWindowTitle();
	exportLogAction->setEnabled(true);

	QWidget* parent = parentWidget();
	parent->show();
	parent->setFocus();
	emit solverStarted();
}
void SolverConsoleWindow::terminateSolver()
{
	if (impl->m_process == nullptr) {return;}
	QMessageBox::StandardButton button =  QMessageBox::question(
																					this,
																					tr("Confirm Solver Termination"),
																					tr("Do you really want to kill the solver?"),
																					QMessageBox::Yes | QMessageBox::No,
																					QMessageBox::No);
	if (QMessageBox::No == button) {return;}

	// In case solver stops before pressing "Yes" button.
	if (impl->m_process == nullptr) {return;}

	terminateSolverSilently();
}

void SolverConsoleWindow::readStderr()
{
	QString data = impl->m_process->readAllStandardError();
	// remove "\r",  "\n"
	data.replace('\r', "").replace('\n', "");
	appendLogLine(data);
}

void SolverConsoleWindow::readStdout()
{
	while (impl->m_process->canReadLine()) {
		QString data = impl->m_process->readLine(200);
		// remove "\r",  "\n"
		data.replace('\r', "").replace('\n', "");
		appendLogLine(data);
	}
}

void SolverConsoleWindow::handleSolverFinish(int, QProcess::ExitStatus status)
{
	delete impl->m_process;
	impl->m_process = nullptr;
	m_projectDataItem->close();

	removeCancelFile();
	removeCancelOkFile();

	if (m_destructing) {return;}

	updateWindowTitle();
	QWidget* parent = parentWidget();
	parent->show();
	parent->setFocus();

	emit solverFinished();

	if (impl->m_iricMainWindow->cuiMode()) {return;}

	if (! impl->m_solverKilled) {
		if (status == 0) {
			// Finished normally.
			QMessageBox::information(this, tr("Solver Finished"), tr("The solver finished calculation."));
		} else {
			// Finished abnormally.
			QMessageBox::warning(this, tr("Solver Finished"), tr("The solver finished abnormally."));
		}
	}
}

void SolverConsoleWindow::updateWindowTitle()
{
	if (m_projectData == nullptr) {
		// Project is not loaded yet.
		setWindowTitle(tr("Solver Console"));
		return;
	}
	QString solver = m_projectData->solverDefinition()->caption();
	QString status;
	if (impl->m_process != nullptr) {
		status = tr("running");
	} else {
		status = tr("stopped");
	}
	setWindowTitle(tr("Solver Console [%1] (%2)").arg(solver, status));
}

void SolverConsoleWindow::setupDefaultGeometry()
{
	QWidget* parent = parentWidget();
	parent->move(30, 30);
	parent->resize(700, 500);
}

void SolverConsoleWindow::copy() const
{
	m_console->copy();
}

QPixmap SolverConsoleWindow::snapshot()
{
	QPixmap pixmap(m_console->size());
	m_console->render(&pixmap);
	return pixmap;
}

void SolverConsoleWindow::clear()
{
	m_console->clear();
	m_projectDataItem->clear();
}

void SolverConsoleWindow::startSolverSilently()
{
	m_projectData->mainfile()->postSolutionInfo()->close();

	QString cgnsname = m_projectData->mainfile()->cgnsFileList()->current()->filename();
	cgnsname.append(".cgn");

	impl->m_process = new QProcess(this);
	QString wd = m_projectData->workDirectory();
	impl->m_process->setWorkingDirectory(wd);

	// set language setting to the environment.
	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QProcessEnvironment env = m_projectData->mainWindow()->processEnvironment();
	env.insert("iRIC_LANG", locale);

	impl->m_process->setProcessEnvironment(env);

	// create connections.
	connect(impl->m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
	connect(impl->m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
	connect(impl->m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleSolverFinish(int, QProcess::ExitStatus)));

	QStringList args;
	args << cgnsname;

	// remove cancel file.
	removeCancelFile();
	// remove cancel_ok file.
	removeCancelOkFile();

	impl->m_solverKilled = false;

	QString solver = m_projectData->solverDefinition()->executableFilename();
	impl->m_process->start(solver, args);
}

void SolverConsoleWindow::terminateSolverSilently()
{
	if (impl->m_process == nullptr) {return;}

	impl->m_solverKilled = true;
	QString wd = m_projectData->workDirectory();
	QFile cancelOkFile(QDir(wd).absoluteFilePath(".cancel_ok"));
	if (cancelOkFile.exists()) {
		// this solver supports canceling through ".cancel". Create ".cancel".
		createCancelFile();
		// wait for 30 secs.
		impl->m_process->waitForFinished();
	} else {
		// this solver does not supports canceling through ".cancel". Kill the solver.
		impl->m_process->kill();
	}
}

void SolverConsoleWindow::waitForSolverFinish()
{
	impl->m_process->waitForFinished(-1);
}

void SolverConsoleWindow::appendLogLine(const QString& line)
{
	m_projectDataItem->append(line);
}

void SolverConsoleWindow::editBackgroundColor()
{
	QColor c = backgroundColor();
	QColor newc = QColorDialog::getColor(c, this, tr("Background Color"));
	if (! newc.isValid()) {return;}

	iRICUndoStack::instance().push(new SetBackgroundColorCommand(newc, this));
}

QColor SolverConsoleWindow::backgroundColor() const
{
	QPalette p = m_console->palette();
	return p.color(QPalette::Base);
}

void SolverConsoleWindow::setBackgroundColor(const QColor& c)
{
	QPalette p = m_console->palette();
	p.setColor(QPalette::Base, c);
	m_console->setPalette(p);
}

void SolverConsoleWindow::closeEvent(QCloseEvent* e)
{
	parentWidget()->hide();
	e->ignore();
}

void SolverConsoleWindow::createCancelFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelFile(QDir(wd).absoluteFilePath(".cancel"));
	cancelFile.open(QFile::WriteOnly);
	cancelFile.close();
}

void SolverConsoleWindow::removeCancelFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelFile(QDir(wd).absoluteFilePath(".cancel"));
	cancelFile.remove();
}

void SolverConsoleWindow::removeCancelOkFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelOkFile(QDir(wd).absoluteFilePath(".cancel_ok"));
	cancelOkFile.remove();
}
