#ifndef SOLVERCONSOLEWINDOW_H
#define SOLVERCONSOLEWINDOW_H

#include "solverconsole_global.h"
#include <guicore/base/clipboardoperatablewindow.h>
#include <guicore/base/snapshotenabledwindow.h>
#include <guicore/base/windowwithzindex.h>

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

/// SolverConsoleWindow class represents the console window
/// that monitor the progress of solver calculation by showing
/// the STDOUT of solver.
class SOLVERCONSOLEDLL_EXPORT SolverConsoleWindow :
	public QMainWindow,
	public ClipboardOperatableWindow,
	public SnapshotEnabledWindow,
	public WindowWithZIndex
{
	Q_OBJECT

public:
	/// Constructor
	SolverConsoleWindow(){init();}
	/// Constructor
	SolverConsoleWindow(QWidget* /*parent*/){init();}
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
	bool acceptCopy(){return true;}
	void copy();
	/// Clear the console log.
	void clear();

	QPixmap snapshot();

	QAction* exportLogAction;

public slots:
	/// Start the solver execution
	void startSolver();
	/// Terminate the solver execution
	void terminateSolver();
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
	void appendLogLine(const QString& line);
	/// Background color
	const QColor backgroundColor() const;
	/// Set background color;
	void setBackgroundColor(const QColor& c);
	SolverConsoleWindowProjectDataItem* m_projectDataItem;
	ProjectData* m_projectData;
	QPlainTextEdit* m_console;
	QProcess* m_process;
	bool m_solverKilled;
	bool m_destructing;

public:
	friend class SolverConsoleWindowProjectDataItem;
	friend class SolverConsoleWindowBackgroundColorCommand;
};

#endif // SOLVERCONSOLEWINDOW_H
