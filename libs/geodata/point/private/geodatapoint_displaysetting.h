#ifndef GEODATAPOINT_DISPLAYSETTING_H
#define GEODATAPOINT_DISPLAYSETTING_H

#include "../geodatapoint.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/opacitycontainer.h>

class GeoDataPoint::DisplaySetting : public CompositeContainer
{
public:
	enum class Mapping {Value, Arbitrary};
	enum class Shape {Point, Image};

	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	EnumContainerT<Mapping> mapping;
	EnumContainerT<Shape> shape;
	ColorContainer color;
	OpacityContainer opacity;
	IntContainer pointSize;
	QImage image;
	IntContainer imageMaxSize;
};

#endif // GEODATAPOINT_DISPLAYSETTING_H
