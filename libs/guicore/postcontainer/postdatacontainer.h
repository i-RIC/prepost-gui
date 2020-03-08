#ifndef POSTDATACONTAINER_H
#define POSTDATACONTAINER_H

#include "../guicore_global.h"
#include "../project/projectdataitem.h"

class PostSolutionInfo;

class GUICOREDLL_EXPORT PostDataContainer : public ProjectDataItem
{
	Q_OBJECT

public:
	PostDataContainer(PostSolutionInfo* parent);
	virtual bool handleCurrentStepUpdate(const int fn, const int timeStep);
	PostSolutionInfo* solutionInfo() const;

signals:
	void dataUpdated();
};

#endif // POSTDATACONTAINER_H
