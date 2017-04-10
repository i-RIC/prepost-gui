#ifndef GRIDCREATINGCONDITIONFACTORYI_H
#define GRIDCREATINGCONDITIONFACTORYI_H

class GridCreatingCondition;
class ProjectDataItem;

class QDomNode;

class GridCreatingConditionFactoryI
{
public:
	virtual ~GridCreatingConditionFactoryI() {}

	virtual GridCreatingCondition* restore(const QDomNode& node, ProjectDataItem* item) const = 0;
};


#endif // GRIDCREATINGCONDITIONFACTORYI_H
