#ifndef STARTPAGERECENTSOLVERLIST_H
#define STARTPAGERECENTSOLVERLIST_H

#include <QWidget>

class SolverDefinitionList;
class SolverDefinitionAbstract;

class QVBoxLayout;

class StartPageRecentSolverList : public QWidget
{
	Q_OBJECT

private:
	const static int MAXSOLVERS = 6;

public:
	explicit StartPageRecentSolverList(QWidget* parent = nullptr);
	void setSolverList(SolverDefinitionList* solverList);

signals:
	void solverSelected(SolverDefinitionAbstract* solverDef);

private slots:
	void handleSolverSelection();

private:
	void add(SolverDefinitionAbstract* solverDef);
	QVBoxLayout* m_layout;
	int m_numberOfSolvers;
};

#endif // STARTPAGERECENTSOLVERLIST_H
