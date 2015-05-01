#ifndef SOLVERDEFINITIONLIST_H
#define SOLVERDEFINITIONLIST_H

#include <QObject>
#include <QList>
#include <QLocale>

class QAction;
class SolverDefinitionAbstract;
class SolverDefinitionListDialog;
class ProjectData;

/// This class represents the Solver definition file list currently installed in iRIC.
/**
 * The main role of this class is to provide the list of solvers ( solverList() ),
 * that is updated automatically, and emits updated() signal when the list is updated.
 */
class SolverDefinitionList : public QObject
{
	Q_OBJECT
public:
	/// constructor
	/**
	 * @param installDir install directory of iRIC
	 */
	SolverDefinitionList(const QString& installDir, const QLocale& locale, QObject* parent);
	/// destructor
	virtual ~SolverDefinitionList() {clean();}
	/// Return absolute path of the folder that contains the solver information.
	QString absoluteSolverPath(const QString& solverFolder);
	/// The list of solvers;
	QList<SolverDefinitionAbstract*> solverList() {return m_solverList;}
	/// The list of actions to add in File -&gt; New Simulation menu
	QList<QAction*> actionList();
	/// The dialog to show solver definition list
	SolverDefinitionListDialog* dialog(QWidget* parent);
	const QString supportingSolverFolder(ProjectData* p);
protected slots:
	/// Update solver list
	void updateSolverList();
private:
	/// free the memory.
	void clean();
signals:
	/// This signal is emitted when the list of solver is updated (for example
	/// when you added or removed a solver).
	void updated();
	/// This signal is emitted when the list of solver is updated (for example
	/// when you added or removed a solver).
	void updated(SolverDefinitionList*);
private:
	/// A vector that stores the list of SolverDefinition Abstract.
	QList<SolverDefinitionAbstract*> m_solverList;
	/// Full path of the directory that contains solvers.
	/**
	 * please refer to @ref dirstructure for the directory structure
	 * under install directory of iRIC.
	 */
	QString m_targetDirectory;
	/// Locale currently used.
	QLocale m_locale;
	/// The dialog to show solver definition list
	SolverDefinitionListDialog* m_dialog;
};

#endif // SOLVERDEFINITIONLIST_H
