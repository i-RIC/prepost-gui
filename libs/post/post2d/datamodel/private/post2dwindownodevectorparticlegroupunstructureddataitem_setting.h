#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTING_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTING_H

#include "../post2dwindownodevectorparticlegroupunstructureddataitem.h"

#include <guicore/region/region2dsettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/qpointfcontainer.h>

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting : public CompositeContainer
{
public:
	class StartPosition : public CompositeContainer {
	public:
		StartPosition();
		StartPosition(const StartPosition& pos);

		StartPosition& operator=(const StartPosition& pos);
		XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

		QPointFContainer point1;
		QPointFContainer point2;
		BoolContainer pointsSet;
		IntContainer numberOfPoints;

		ColorContainer color;
		IntContainer pointSize;
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

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTING_H
