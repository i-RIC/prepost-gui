#ifndef POSTSERIESDATACONTAINER_H
#define POSTSERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postsolutioninfo.h"
#include "postdatacontainer.h"

#include <vector>

class GUICOREDLL_EXPORT PostSeriesDataContainer : public PostDataContainer
{

public:
	PostSeriesDataContainer(PostSolutionInfo::Dimension dim, PostSolutionInfo* parent);
	const std::vector<double>& data() const;
	int update();
	int loadFromCgnsFile() override;

protected:
	std::vector<double> m_data;
	PostSolutionInfo::Dimension m_dimension;

private:
	virtual int loadData() = 0;
};

#endif // POSTSERIESDATACONTAINER_H
