#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H

#include "../post2dwindownodevectorstreamlinegroupstructureddataitem.h"

#include <guicore/filter/structuredgridskiporsubdividesettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <guicore/region/regionrange2dsettingcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting : public CompositeContainer
{
public:
	class StartPosition : public CompositeContainer {
	public:
		StartPosition();
		StartPosition(const StartPosition& pos);

		StartPosition& operator=(const StartPosition& pos);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

		StructuredGridSkipOrSubdivideSettingContainer skipOrSubdivide;
		RegionRange2dSettingContainer range;

		ColorContainer color;
		IntContainer width;
	};

	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	void copyValue(const XmlAttributeContainer& setting) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	Region2dSettingContainer region;
	std::vector<StartPosition> startPositions;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H
