#ifndef GRIDCREATINGCONDITIONFACTORY_H
#define GRIDCREATINGCONDITIONFACTORY_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

#include <guicore/pre/gridcreatingcondition/gridcreatingconditionfactoryi.h>

class GridCreatingConditionCreator;
class GridCreatingCondition;
class ProjectDataItem;
class QDomNode;
class SolverDefinitionGridType;

class GridCreatingConditionFactory : public QObject, public GridCreatingConditionFactoryI
{

private:
	/// Constructor
	/**
	 * this function is made to be private (Singleton pattern)
	 */
	GridCreatingConditionFactory(QWidget* mainWindow);

public:
	static GridCreatingConditionFactory& instance(QWidget* mainWindow);
	const QList<GridCreatingConditionCreator*> compatibleCreators(const SolverDefinitionGridType& gridType) const;
	GridCreatingConditionCreator* getCreator(const QString& name) const;
	GridCreatingCondition* restore(const QDomNode& node, ProjectDataItem* item) const override;
	void setMainWindow(QWidget* mw);

private:
	void setupNameMap();

	QList<GridCreatingConditionCreator*> m_creators;
	QMap<QString, GridCreatingConditionCreator*> m_creatorNameMap;
	QWidget* m_mainWindow;
	static GridCreatingConditionFactory* m_instance;
};

#endif // GRIDCREATINGCONDITIONFACTORY_H
