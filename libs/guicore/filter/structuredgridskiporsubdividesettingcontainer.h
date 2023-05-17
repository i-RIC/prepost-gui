#ifndef STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGCONTAINER_H
#define STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class GUICOREDLL_EXPORT StructuredGridSkipOrSubdivideSettingContainer : public CompositeContainer
{
public:
	enum class SamplingMode {
		Normal,
		Skip,
		Subdivide
	};
	enum class Dimension {
		Two,
		Three,
	};

	StructuredGridSkipOrSubdivideSettingContainer();
	StructuredGridSkipOrSubdivideSettingContainer(const StructuredGridSkipOrSubdivideSettingContainer& s);
	~StructuredGridSkipOrSubdivideSettingContainer();

	StructuredGridSkipOrSubdivideSettingContainer& operator=(const StructuredGridSkipOrSubdivideSettingContainer& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	vtkStructuredGrid* buildSampledData(vtkStructuredGrid* data, Dimension dimension) const;

	EnumContainerT<SamplingMode> samplingMode;
	IntContainer skipRate;
	IntContainer subDivideRate;
};

#endif // STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGCONTAINER_H
