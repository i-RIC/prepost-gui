#ifndef POSTSERIESDATACONTAINER_H
#define POSTSERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postsolutioninfo.h"
#include "postdatacontainer.h"

class GUICOREDLL_EXPORT PostSeriesDataContainer : public PostDataContainer
{

public:
	PostSeriesDataContainer(PostSolutionInfo::Dimension dim, PostSolutionInfo* parent);
	const QList<double>& data() const;
	bool handleCurrentStepUpdate(const int fn, const int timeStep) override;
	void update(const int fn);
	void loadFromCgnsFile(const int fn) override;

protected:
	bool setBaseId(const int fn);
	virtual bool loadData(const int fn) = 0;
	QList<double> m_data;
	PostSolutionInfo::Dimension m_dimension;
	int m_baseId;
	int m_cellDim;
};

#endif // POSTSERIESDATACONTAINER_H
