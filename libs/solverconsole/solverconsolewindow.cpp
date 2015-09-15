#include "solverconsolewindow.h"
#include "solverconsolewindowprojectdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
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
#include <QFileDialog>
#include <QFont>
#include <QIODevice>
#include <QLocale>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QUndoCommand>

SolverConsoleWindow::~SolverConsoleWindow()
{

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

	m_process = nullptr;

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
	return m_process != nullptr;
}

void SolverConsoleWindow::startSolver()
{
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

	QString cgnsname = m_projectData->mainfile()->cgnsFileList()->current()->filename();
	cgnsname.append(".cgn");

	m_process = new QProcess(this);
	QString wd = m_projectData->workDirectory();
	m_process->setWorkingDirectory(wd);

	// set language setting to the environment.
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QProcessEnvironment env = m_projectData->mainWindow()->processEnvironment();
	env.insert("iRIC_LANG", locale);

	m_process->setProcessEnvironment(env);

	// create connections.
	connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
	connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleSolverFinish(int, QProcess::ExitStatus)));

	QStringList args;
	args << cgnsname;

	// remove cancel file.
	removeCancelFile();
	// remove cancel_ok file.
	removeCancelOkFile();

	m_solverKilled = false;
	m_process->start(solver, args);
	updateWindowTitle();
	exportLogAction->setEnabled(true);

	QWidget* parent = parentWidget();
	parent->show();
	parent->setFocus();
	emit solverStarted();
}
void SolverConsoleWindow::terminateSolver()
{
	if (m_process == nullptr) {return;}
	QMessageBox::StandardButton button =  QMessageBox::question(
																					this,
																					tr("Confirm Solver Termination"),
																					tr("Do you really want to kill the solver?"),
																					QMessageBox::Yes | QMessageBox::No,
																					QMessageBox::No);
	if (QMessageBox::No == button) {
		return;
	}
	// In case solver stops before pressing "Yes" button.
	if (m_process == nullptr) { return; }

	m_solverKilled = true;
	QString wd = m_projectData->workDirectory();
	QFile cancelOkFile(QDir(wd).absoluteFilePath(".cancel_ok"));
	if (cancelOkFile.exists()) {
		// this solver supports canceling through ".cancel". Create ".cancel".
		createCancelFile();
		// wait for 30 secs.
		m_process->waitForFinished();
	} else {
		// this solver does not supports canceling through ".cancel". Kill the solver.
		m_process->kill();
	}
}

void SolverConsoleWindow::readStderr()
{
	QString data = m_process->readAllStandardError();
	// remove "\r",  "\n"
	data.replace('\r', "").replace('\n', "");
	appendLogLine(data);
}

void SolverConsoleWindow::readStdout()
{
	while (m_process->canReadLine()) {
		QString data = m_process->readLine(200);
		// remove "\r",  "\n"
		data.replace('\r', "").replace('\n', "");
		appendLogLine(data);
	}
}

void SolverConsoleWindow::handleSolverFinish(int, QProcess::ExitStatus status)
{
	delete m_process;
	m_process = nullptr;
	m_projectDataItem->close();

	// remove cancel file.
	removeCancelFile();
	// remove cancel_ok file.
	removeCancelOkFile();

	if (m_destructing) {return;}
	updateWindowTitle();
	QWidget* parent = parentWidget();
	parent->show();
	parent->setFocus();

	emit solverFinished();
	if (! m_solverKilled) {
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
	ProjectData* data = m_projectData;
	if (data == nullptr) {
		// Project is not loaded yet.
		setWindowTitle(QString(tr("Solver Console")));
		return;
	}
	QString solver = m_projectData->solverDefinition()->caption();
	QString status;
	if (m_process != nullptr) {
		status = tr("running");
	} else {
		status = tr("stopped");
	}
	setWindowTitle(QString(tr("Solver Console [%1] (%2)")).arg(solver, status));
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

void SolverConsoleWindow::appendLogLine(const QString& line)
{
	m_projectDataItem->append(line);
}

/// Command to edit background color of solver console window
class SolverConsoleWindowBackgroundColorCommand : public QUndoCommand
{
public:
	SolverConsoleWindowBackgroundColorCommand(QColor newc, QColor oldc, SolverConsoleWindow* w)
		: QUndoCommand(QObject::tr("Background Color Setting")) {
		m_newColor = newc;
		m_oldColor = oldc;
		m_window = w;
	}
	void redo() {
		m_window->setBackgroundColor(m_newColor);
	}
	void undo() {
		m_window->setBackgroundColor(m_oldColor);
	}
private:
	QColor m_newColor;
	QColor m_oldColor;
	SolverConsoleWindow* m_window;
};

void SolverConsoleWindow::editBackgroundColor()
{
	QColor c = backgroundColor();
	QColor newc = QColorDialog::getColor(c, this, tr("Background Color"));
	if (! newc.isValid()) {return;}
	iRICUndoStack::instance().push(new SolverConsoleWindowBackgroundColorCommand(newc, c, this));
}

const QColor SolverConsoleWindow::backgroundColor() const
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
