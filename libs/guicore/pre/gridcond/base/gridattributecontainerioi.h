#ifndef GRIDATTRIBUTECONTAINERIOI_H
#define GRIDATTRIBUTECONTAINERIOI_H

#include <h5cgnsgridattributes.h>

class GridAttributeContainer;

class GridAttributeContainerIoI
{
public:
	virtual ~GridAttributeContainerIoI() {};

	virtual int loadFromCgnsFile(const iRICLib::H5CgnsGridAttributes& atts, GridAttributeContainer* cont) = 0;
	virtual int saveToCgnsFile(iRICLib::H5CgnsGridAttributes* atts, GridAttributeContainer* cont) = 0;
	virtual void clearTemporaryData(GridAttributeContainer* cont) = 0;
	virtual void handleDimensionCurrentIndexChange(int oldIndex, int newIndex, GridAttributeContainer* cont) = 0;
};

#endif // GRIDATTRIBUTECONTAINERIOI_H
