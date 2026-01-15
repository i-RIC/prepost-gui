#ifndef POST2DWINDOWBCDATAITEM_H
#define POST2DWINDOWBCDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

class BoundaryConditionDialog;
class v4PostZoneDataBC;

class Post2dWindowBCDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, v4PostZoneDataBC* inputGridBC, Post2dWindowDataItem* parent);
	~Post2dWindowBCDataItem();

private:
	void setupActors();
	void doUpdateActorSetting() override;
	void updateElements();
	
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;

	class Setting;
};

#endif // POST2DWINDOWBCDATAITEM_H
