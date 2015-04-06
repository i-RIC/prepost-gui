#ifndef GRIDCREATINGCONDITIONCREATOR_H
#define GRIDCREATINGCONDITIONCREATOR_H

#include "../../guicore_global.h"
#include "../../solverdef/solverdefinitiongridtype.h"
#include <QObject>

class QDomNode;
class QWidget;
class QToolBar;
class ProjectDataItem;
class GridCreatingCondition;
class GridCreatingConditionFactory;
class PreProcessorGridCreatingConditionDataItemInterface;

class GUICOREDLL_EXPORT GridCreatingConditionCreator : public QObject
{
public:
	GridCreatingConditionCreator();
	virtual ~GridCreatingConditionCreator(){}
	/// The type of grid that this condition can create.
	virtual SolverDefinitionGridType::GridType gridType() const = 0;
	virtual GridCreatingCondition* create(ProjectDataItem* parent) = 0;
	GridCreatingCondition* restore(const QDomNode& node, ProjectDataItem* parent);
	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	const QString& description() const {return m_description;}
	virtual bool checkAvailability(PreProcessorGridCreatingConditionDataItemInterface* ditem);

protected:
	QString m_name;
	QString m_caption;
	QString m_description;
};

#endif // GRIDCREATINGCONDITIONCREATOR_H
