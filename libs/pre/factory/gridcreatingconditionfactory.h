#ifndef GRIDCREATINGCONDITIONFACTORY_H
#define GRIDCREATINGCONDITIONFACTORY_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <guicore/solverdef/solverdefinitiongridtype.h>

class GridCreatingConditionCreator;
class GridCreatingCondition;
class ProjectDataItem;
class QDomNode;

class GridCreatingConditionFactory : public QObject
{

private:
	/// Constructor
	/**
	 * this function is made to be private (Singleton pattern)
	 */
	GridCreatingConditionFactory(QWidget* mainWindow);

public:
	static GridCreatingConditionFactory& instance(QWidget* mainWindow) {
		if (m_instance == nullptr) {
			m_instance = new GridCreatingConditionFactory(mainWindow);
		}
		return *m_instance;
	}
	const QList<GridCreatingConditionCreator*> compatibleCreators(SolverDefinitionGridType::GridType gt) const;
	GridCreatingConditionCreator* getCreator(const QString& name) const;
	GridCreatingCondition* restore(const QDomNode& node, ProjectDataItem* item) const;
	void setMainWindow(QWidget* mw) {m_mainWindow = mw;}

private:
	void setupNameMap();

	QList<GridCreatingConditionCreator*> m_creators;
	QMap<QString, GridCreatingConditionCreator*> m_creatorNameMap;
	QWidget* m_mainWindow;
	static GridCreatingConditionFactory* m_instance;
};

#endif // GRIDCREATINGCONDITIONFACTORY_H
