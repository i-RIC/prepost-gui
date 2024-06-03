#ifndef GEODATAPOINTGROUP_DISPLAYSETTING_H
#define GEODATAPOINTGROUP_DISPLAYSETTING_H

#include "../geodatapointgroup.h"
#include "geodatapointgroup_scalesizepair.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/opacitycontainer.h>

class GeoDataPointGroup::DisplaySetting : public CompositeContainer
{
public:
	enum class Mapping {Value, Arbitrary};
	enum class Shape {Point, Image};
	enum class AnchorPosition {
		Center,
		Top,
		Bottom,
		Left,
		Right,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	EnumContainerT<Mapping> mapping;
	EnumContainerT<Shape> shape;
	EnumContainerT<AnchorPosition> anchorPosition;
	ColorContainer color;
	OpacityContainer opacity;
	IntContainer pointSize;
	QImage image;
	IntContainer imageMaxSize;

	std::vector<ScaleSizePair> scaleSizePairs;
};

#endif // GEODATAPOINTGROUP_DISPLAYSETTING_H
