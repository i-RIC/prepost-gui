#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H

#include "../post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include "../../post3dwindowfacesettingcontainer.h"

#include <guicore/filter/structuredgridskiporsubdividesettingcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting : public CompositeContainer
{
public:
	class StartPosition : public CompositeContainer {
	public:
		StartPosition();
		StartPosition(const StartPosition& pos);

		StartPosition& operator=(const StartPosition& pos);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

		StructuredGridSkipOrSubdivideSettingContainer skipOrSubdivide;
		Post3dWindowFaceSettingContainer face;

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

	std::vector<StartPosition> startPositions;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTING_H
