#ifndef GRIDATTRIBUTECONTAINERIOPOST_H
#define GRIDATTRIBUTECONTAINERIOPOST_H

#include "../../../guicore_global.h"
#include "gridattributecontainerioi.h"

class PostSolutionInfo;

class GUICOREDLL_EXPORT GridAttributeContainerIoPost : public GridAttributeContainerIoI
{
public:
	GridAttributeContainerIoPost(int dim, const std::string& zoneName, PostSolutionInfo* info);

	int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts, GridAttributeContainer* cont) override;
	int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts, GridAttributeContainer* cont) override;
	void clearTemporaryData(GridAttributeContainer* cont) override;
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex, GridAttributeContainer* cont) override;

private:
	int m_dim;
	std::string m_zoneName;
	PostSolutionInfo* m_solutionInfo;
};

#endif // GRIDATTRIBUTECONTAINERIOPOST_H
