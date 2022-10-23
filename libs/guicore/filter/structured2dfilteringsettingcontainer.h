#ifndef STRUCTURED2DFILTERINGSETTINGCONTAINER_H
#define STRUCTURED2DFILTERINGSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class vtkStructuredGrid;

class GUICOREDLL_EXPORT Structured2dFilteringSettingContainer : public CompositeContainer
{
public:
	enum class SamplingMode {
		All  = 1,
		Rate = 2,
	};

	Structured2dFilteringSettingContainer();
	Structured2dFilteringSettingContainer(const Structured2dFilteringSettingContainer& c);

	Structured2dFilteringSettingContainer& operator=(const Structured2dFilteringSettingContainer& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	vtkStructuredGrid* buildSampledData(vtkStructuredGrid* data) const;

	EnumContainerT<SamplingMode> samplingMode;
	IntContainer iSampleRate;
	IntContainer jSampleRate;
};

#endif // STRUCTURED2DFILTERINGSETTINGCONTAINER_H
