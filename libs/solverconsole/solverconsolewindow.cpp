#include "solverconsolewindow.h"
#include "solverconsolewindowprojectdataitem.h"
#include "private/solverconsolewindow_impl.h"
#include "private/solverconsolewindow_setbackgroundcolorcommand.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/cgnsfileentry.h>
#include <guicore/project/cgnsfilelist.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QCoreApplication>
#include <QColorDialog>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <QTextCodec>

namespace {

const int SOLVER_CANCEL_WAITTIME = 5;

} // namespace

SolverConsoleWindow::Impl::Impl(iRICMainWindowInterface* mainW, SolverConsoleWindow* w) :
	m_iricMainWindow {mainW},
	m_process {nullptr},
	m_window {w}
{}

void SolverConsoleWindow::Impl::init()
{
	m_window->setMinimumSize(480, 360);
	m_projectData = nullptr;
	m_projectDataItem = nullptr;
	m_destructing = false;

	m_window->exportLogAction = new QAction(SolverConsoleWindow::tr("&Export solver console log..."), m_window);
	m_window->exportLogAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	m_window->exportLogAction->setDisabled(true);

	m_console = new QPlainTextEdit(m_window);
	m_console->setReadOnly(true);
	m_console->setAutoFillBackground(true);
	m_console->setWordWrapMode(QTextOption::WrapAnywhere);
	QFont font("Courier");
	font.setStyleHint(QFont::Courier);
	font.setPointSize(9);
	m_console->setFont(font);
	m_window->setCentralWidget(m_console);

	m_window->updateWindowTitle();
}

void SolverConsoleWindow::Impl::createCancelFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelFile(QDir(wd).absoluteFilePath(".cancel"));
	cancelFile.open(QFile::WriteOnly);
	cancelFile.close();
}

void SolverConsoleWindow::Impl::removeCancelFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelFile(QDir(wd).absoluteFilePath(".cancel"));
	cancelFile.remove();
}

void SolverConsoleWindow::Impl::removeCancelOkFile()
{
	QString wd = m_projectData->workDirectory();
	QFile cancelOkFile(QDir(wd).absoluteFilePath(".cancel_ok"));
	cancelOkFile.remove();
}

void SolverConsoleWindow::Impl::appendLogLine(const QString& line)
{
	m_projectDataItem->append(line);
}

// public interfaces

SolverConsoleWindow::SolverConsoleWindow(iRICMainWindowInterface* parent) :
	QMainWindow {parent},
	impl {new Impl(parent, this)}
{
	impl->init();
}

SolverConsoleWindow::~SolverConsoleWindow()
{
	delete impl;
}

void SolverConsoleWindow::setProjectData(ProjectData* d)
{
	impl->m_projectDataItem = new SolverConsoleWindowProjectDataItem(this, d->mainfile());
	connect(exportLogAction, SIGNAL(triggered()), impl->m_projectDataItem, SLOT(exportConsoleLog()));
}

SolverConsoleWindowProjectDataItem* SolverConsoleWindow::projectDataItem()
{
	return impl->m_projectDataItem;
}

bool SolverConsoleWindow::isSolverRunning()
{
	return impl->m_process != nullptr;
}

void SolverConsoleWindow::startSolver()
{
	// Check post only mode
	if (impl->m_projectData->isPostOnlyMode()) {
		QMessageBox::information(this, tr("Information"), tr("This project is opened in post only mode. You can not run the solver."));
		return;
	}

	// Check calculation condition status
	impl->m_projectData->mainWindow()->preProcessorWindow()->checkCalculationConditionImportSourceUpdate();
	// Check grid is ready
	bool ok = impl->m_projectData->mainWindow()->preProcessorWindow()->checkMappingStatus();
	if (! ok) {return;}

	// Check grid shape
	QSettings settings;
	QString msg = impl->m_projectData->mainWindow()->preProcessorWindow()->checkGrid(settings.value("gridcheck/beforeexec", true).value<bool>());
	if (! msg.isEmpty()) {
		QString logFileName = impl->m_projectData->absoluteFileName("gridcheck.txt");
		msg.append(QString("<a href=\"%1\">").arg(QString("file:///").append(logFileName))).append(tr("Show Detail")).append("</a>");
		int ret = QMessageBox::warning(impl->m_projectData->mainWindow(), tr("Warning"), tr("The following problems found in the grid(s). Do you really want to run the solver with this grid?") + msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	// If the cgns file already has results, clear them first.
	if (impl->m_projectData->mainfile()->hasResults() && QMessageBox::Cancel == QMessageBox::warning(this, tr("The simulation has result"), tr("Current simulation already has result data. When you run the solver, the current result data is discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
		return;
	}
	// discard result, and save now.
	try {
		impl->m_projectData->mainfile()->clearResults();
	} catch (ErrorMessage& m) {
		QMessageBox::warning(this, tr("Warning"), tr("Error occured. %1").arg(m));
		return;
	}

	int ret = QMessageBox::information(this, tr("Information"), tr("We recommend that you save the project before starting the solver. Do you want to save?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
	if (ret == QMessageBox::Yes) {
		// save the project file.
		if (! impl->m_projectData->mainWindow()->saveProject()) {return;}
	} else if (ret == QMessageBox::Cancel) {
		return;
	}
	clear();

	// check solver executable existance
	QString solver = impl->m_projectData->solverDefinition()->executableFilename();
	QFile solverExec(solver);
	if (! solverExec.exists()) {
		// solver executable does not exists!!
		QMessageBox::critical(this, tr("Error"), tr("Solver executable file %1 does not exists.").arg(solver));
		return;
	}

	impl->m_projectDataItem->open();

	impl->m_projectData->setIsSolverRunning(true);
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
	QTextCodec* codec = QTextCodec::codecForLocale();

	auto byteArray = impl->m_process->readAllStandardError();
	auto data = codec->toUnicode(byteArray);

	// remove "\r",  "\n"
	data.replace('\r', "").replace('\n', "");
	impl->appendLogLine(data);
}

void SolverConsoleWindow::readStdout()
{
	QTextCodec* codec = QTextCodec::codecForLocale();

	while (impl->m_process->canReadLine()) {
		auto byteArray = impl->m_process->readLine(200);
		auto data = codec->toUnicode(byteArray);
		// remove "\r",  "\n"
		data.replace('\r', "").replace('\n', "");
		impl->appendLogLine(data);
	}
}

void SolverConsoleWindow::handleSolverFinish(int, QProcess::ExitStatus status)
{
	delete impl->m_process;
	impl->m_process = nullptr;
	impl->m_projectDataItem->close();
	impl->m_projectData->setIsSolverRunning(false);

	impl->removeCancelFile();
	impl->removeCancelOkFile();

	if (impl->m_destructing) {return;}

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
	if (impl->m_projectData == nullptr) {
		// Project is not loaded yet.
		setWindowTitle(tr("Solver Console"));
		return;
	}
	QString solver = impl->m_projectData->solverDefinition()->caption();
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
	impl->m_console->copy();
}

QPixmap SolverConsoleWindow::snapshot()
{
	QPixmap pixmap(impl->m_console->size());
	impl->m_console->render(&pixmap);
	return pixmap;
}

void SolverConsoleWindow::clear()
{
	impl->m_console->clear();
	impl->m_projectDataItem->clear();
}

void SolverConsoleWindow::applyPreferenceSetting()
{
	if (impl->m_projectDataItem) {
		impl->m_projectDataItem->loadExternalData();
	}
}

void SolverConsoleWindow::startSolverSilently()
{
	impl->m_projectData->mainfile()->postSolutionInfo()->close();

	QString cgnsname = impl->m_projectData->mainfile()->cgnsFileList()->current()->filename();
	cgnsname.append(".cgn");

	impl->m_process = new QProcess(this);
	QString wd = impl->m_projectData->workDirectory();
	impl->m_process->setWorkingDirectory(wd);
	impl->m_process->setProcessChannelMode(QProcess::MergedChannels);

	// set language setting to the environment.
	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QProcessEnvironment env = impl->m_projectData->mainWindow()->processEnvironment();
	env.insert("iRIC_LANG", locale);

	impl->m_process->setProcessEnvironment(env);

	// create connections.
	connect(impl->m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
	connect(impl->m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
	connect(impl->m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleSolverFinish(int, QProcess::ExitStatus)));

	QStringList args;
	args << cgnsname;

	impl->removeCancelFile();
	impl->removeCancelOkFile();

	impl->m_solverKilled = false;

	QString solver = impl->m_projectData->solverDefinition()->executableFilename();
	impl->m_process->start(solver, args);
}

void SolverConsoleWindow::terminateSolverSilently()
{
	if (impl->m_process == nullptr) {return;}

	impl->m_solverKilled = true;
	QString wd = impl->m_projectData->workDirectory();
	QFile cancelOkFile(QDir(wd).absoluteFilePath(".cancel_ok"));
	if (cancelOkFile.exists()) {
		impl->m_iricMainWindow->enterModelessDialogMode();
		// this solver supports canceling through ".cancel". Create ".cancel".
		impl->createCancelFile();
		// wait for SOLVER_CANCEL_WAITTIME secs.
		int waited = 0;
		while (true) {
			bool ok = impl->m_process->waitForFinished(1000);
			qApp->processEvents();
			if (ok) {break;}
			++ waited;

			if (waited == SOLVER_CANCEL_WAITTIME) {
				QMessageBox::StandardButton button = QMessageBox::question(
					this, tr("Confirm Solver Termination"),
					tr("%1 seconds have passed, but the solver do not end. Do you want to kill the solver?").arg(SOLVER_CANCEL_WAITTIME),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
				if (QMessageBox::Yes == button) {
					if (impl->m_process != nullptr) {
						impl->m_process->kill();
					}
					break;
				}
				waited = 0;
			}
		}
		impl->m_iricMainWindow->exitModelessDialogMode();
	} else {
		// this solver does not supports canceling through ".cancel". Kill the solver.
		impl->m_process->kill();
	}
}

void SolverConsoleWindow::waitForSolverFinish()
{
	impl->m_process->waitForFinished(-1);
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
	QPalette p = impl->m_console->palette();
	return p.color(QPalette::Base);
}

void SolverConsoleWindow::setBackgroundColor(const QColor& c)
{
	QPalette p = impl->m_console->palette();
	p.setColor(QPalette::Base, c);
	impl->m_console->setPalette(p);
}

void SolverConsoleWindow::closeEvent(QCloseEvent* e)
{
	parentWidget()->hide();
	e->ignore();
}
