#ifndef GRIDCREATINGCONDITIONFACTORYI_H
#define GRIDCREATINGCONDITIONFACTORYI_H

class GridCreatingCondition;
class GridCreatingConditionCreator;
class ProjectDataItem;

class QDomNode;
class QString;

class GridCreatingConditionFactoryI
{
public:
	virtual ~GridCreatingConditionFactoryI() {}

	virtual GridCreatingConditionCreator* getCreator(const QString& name) const = 0;;
	virtual GridCreatingCondition* restore(const QDomNode& node, ProjectDataItem* item) const = 0;
};


#endif // GRIDCREATINGCONDITIONFACTORYI_H
