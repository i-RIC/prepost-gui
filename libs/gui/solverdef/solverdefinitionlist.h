#ifndef SOLVERDEFINITIONLIST_H
#define SOLVERDEFINITIONLIST_H

#include <QObject>
#include <QLocale>

#include <vector>

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
	/**
	 * @param installDir install directory of iRIC
	 */
	SolverDefinitionList(const QString& targetDir, const QLocale& locale, QObject* parent);
	~SolverDefinitionList();

	QString absoluteSolverPath(const QString& solverFolder);


	const std::vector<SolverDefinitionAbstract*>& solverList() const;
	std::vector<QAction*> actionList();

	void showListDialog(QWidget* parent);
	QString supportingSolverFolder(ProjectData* p, QWidget* parent);

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
	std::vector<SolverDefinitionAbstract*> m_solverList;
	/// Full path of the directory that contains solvers.
	/**
	 * please refer to @ref dirstructure for the directory structure
	 * under install directory of iRIC.
	 */
	QString m_targetDirectory;
	QLocale m_locale;
	SolverDefinitionListDialog* m_dialog;
};

#endif // SOLVERDEFINITIONLIST_H
