#ifndef GENERALFILTERINGSETTINGCONTAINER_H
#define GENERALFILTERINGSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class SettingContainer;

class vtkPointSet;

class GUICOREDLL_EXPORT GeneralFilteringSettingContainer : public CompositeContainer
{
public:
	const static int DEFAULT_SAMPLING_RATE;
	const static int DEFAULT_SAMPLING_NUMBER;

	enum class SamplingMode {
		All  = 1,
		Rate = 2,
		Number = 3,
	};

	GeneralFilteringSettingContainer();
	GeneralFilteringSettingContainer(const GeneralFilteringSettingContainer& c);
	~GeneralFilteringSettingContainer();

	GeneralFilteringSettingContainer& operator=(const GeneralFilteringSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	vtkPointSet* buildSampledData(vtkPointSet* data) const;

	EnumContainerT<SamplingMode> samplingMode;
	IntContainer samplingRate;
	IntContainer samplingNumber;
};

#endif // GENERALFILTERINGSETTINGCONTAINER_H
