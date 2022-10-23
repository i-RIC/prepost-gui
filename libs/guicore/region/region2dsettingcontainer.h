#ifndef REGION2DSETTINGCONTAINER_H
#define REGION2DSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class GUICOREDLL_EXPORT Region2dSettingContainer : public CompositeContainer
{
public:
	enum class Mode {
		Full,    ///< Select full region
		Active,  ///< Select the active region (wet region)
		Custom   ///< Select custom region
	};

	Region2dSettingContainer();
	Region2dSettingContainer(const Region2dSettingContainer& s);

	Region2dSettingContainer& operator=(const Region2dSettingContainer& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s) override;

	vtkPointSet* buildNodeFilteredData(vtkPointSet* data);
	vtkPointSet* buildCellFilteredData(vtkPointSet* data);

	EnumContainerT<Mode> mode;

	IntContainer iMin;
	IntContainer iMax;
	IntContainer jMin;
	IntContainer jMax;
};

#endif // REGION2DSETTINGCONTAINER_H
