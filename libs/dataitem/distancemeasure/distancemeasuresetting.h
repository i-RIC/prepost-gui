#ifndef DISTANCEMEASURESETTING_H
#define DISTANCEMEASURESETTING_H

#include <guibase/vtktool/vtklabel2dactor.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/qpointfcontainer.h>

class DistanceMeasureSetting : public CompositeContainer
{
public:
	enum LabelMode {
		Auto,
		Custom
	};

	DistanceMeasureSetting();
	DistanceMeasureSetting(const DistanceMeasureSetting& setting);
	~DistanceMeasureSetting();

	DistanceMeasureSetting& operator=(const DistanceMeasureSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer defined;

	QPointFContainer point1;
	QPointFContainer point2;

	BoolContainer showLabel;
	EnumContainerT<LabelMode> labelMode;
	EnumContainerT<vtkLabel2DActor::LabelPosition> labelPosition;
	vtkTextPropertySettingContainer labelFontSetting;
	StringContainer customLabel;

	BoolContainer showMarkers;
	IntContainer markerSize;

	ColorContainer color;
};

#endif // DISTANCEMEASURESETTING_H
