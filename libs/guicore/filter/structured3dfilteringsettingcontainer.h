#ifndef STRUCTURED3DFILTERINGSETTINGCONTAINER_H
#define STRUCTURED3DFILTERINGSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class vtkStructuredGrid;

class GUICOREDLL_EXPORT Structured3dFilteringSettingContainer : public CompositeContainer
{
public:
	enum class SamplingMode {
		All  = 1,
		Rate = 2,
	};

	Structured3dFilteringSettingContainer();
	Structured3dFilteringSettingContainer(const Structured3dFilteringSettingContainer& c);

	Structured3dFilteringSettingContainer& operator=(const Structured3dFilteringSettingContainer& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	vtkStructuredGrid* buildSampledData(vtkStructuredGrid* data) const;

	EnumContainerT<SamplingMode> samplingMode;
	IntContainer iSampleRate;
	IntContainer jSampleRate;
	IntContainer kSampleRate;
};

#endif // STRUCTURED3DFILTERINGSETTINGCONTAINER_H
