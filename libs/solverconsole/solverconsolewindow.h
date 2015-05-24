#ifndef SOLVERCONSOLEWINDOW_H
#define SOLVERCONSOLEWINDOW_H

#include "solverconsole_global.h"
#include <guicore/base/clipboardoperatablewindowinterface.h>
#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/base/windowwithzindexinterface.h>

#include <QMainWindow>
#include <QCloseEvent>
#include <QByteArray>
#include <QProcess>
#include <QProcessEnvironment>

class QPlainTextEdit;
class ProjectData;
class ProjectDataItem;
class SolverConsoleWindowProjectDataItem;
class SolverConsoleWindowBackgroundColorCommand;
class QAction;

/**
	* @brief The solver console window.
	*
	* The console window monitor the progress of solver calculation by showing
	* the STDOUT of solver.
	*/
class SOLVERCONSOLEDLL_EXPORT SolverConsoleWindow :
	public QMainWindow,
	public ClipboardOperatableWindowInterface,
	public SnapshotEnabledWindowInterface,
	public WindowWithZIndexInterface
{
	Q_OBJECT

public:
	/// Constructor
	SolverConsoleWindow() {init();}
	/// Constructor
	SolverConsoleWindow(QWidget* /*parent*/) {init();}
	/// Destructor
	~SolverConsoleWindow();
	/// Set newly created project data.
	void setProjectData(ProjectData* d);
	SolverConsoleWindowProjectDataItem* projectDataItem();
	/// Implementation of close event.
	void closeEvent(QCloseEvent* e);
	/// Returns true when the solver is running.
	bool isSolverRunning();
	/// Setup window position and size to the default.
	void setupDefaultGeometry();
	/// Update Window title depending on Solver name and status
	void updateWindowTitle();
	/// Returns true to inform this window supports copy().
	bool acceptCopy() override {return true;}
	/// Copy the selected string in solver console window.
	void copy() override;
	/// Clear the console log.
	void clear();

	/// Save smapshot to pixmap
	QPixmap snapshot() override;

	QAction* exportLogAction;

public slots:
	/// Start the solver execution
	void startSolver();
	/// Terminate the solver execution
	void terminateSolver();
	/// Show dialog to edit background color
	void editBackgroundColor();

private slots:
	/// Handle the signal that informs that the solver has output at stderr.
	void readStderr();
	/// Handle the signal that informs that the solver has output at stdout.
	void readStdout();
	/// Handle the signal that informs that the solver has finished calculation.
	void handleSolverFinish(int, QProcess::ExitStatus);

signals:
	/// Solver started the calculation;
	void solverStarted();
	/// Solver finished the calculation;
	void solverFinished();

private:
	/// Initialization
	void init();
	/// create ".cancel" file to ask solver to stop.
	void createCancelFile();
	/// remove ".cancel" file.
	void removeCancelFile();
	/// remove ".cancel_ok" file.
	void removeCancelOkFile();
	/// Append a log line
	void appendLogLine(const QString& line);
	/// Background color
	const QColor backgroundColor() const;
	/// Set background color;
	void setBackgroundColor(const QColor& c);
	SolverConsoleWindowProjectDataItem* m_projectDataItem;
	/// ProjectData instance
	ProjectData* m_projectData;
	/// Plain text edit widget to display solver STDOUT output
	QPlainTextEdit* m_console;
	QProcess* m_process;
	bool m_solverKilled;
	bool m_destructing;

public:
	friend class SolverConsoleWindowProjectDataItem;
	friend class SolverConsoleWindowBackgroundColorCommand;
};

#endif // SOLVERCONSOLEWINDOW_H
