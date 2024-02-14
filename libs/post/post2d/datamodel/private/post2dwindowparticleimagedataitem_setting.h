#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTING_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTING_H

#include "../post2dwindowparticleimagedataitem.h"
#include "post2dwindowparticleimagedataitem_valuesizepair.h"

#include <guicore/image/imagesettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

#include <QImage>

#include <vector>

class Post2dWindowParticleImageDataItem::Setting : public CompositeContainer
{
public:
	enum class SizeTarget {Width, Height};
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


	Setting();
	Setting(const Setting& setting);
	~Setting();

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	void copyValue(const XmlAttributeContainer& c) override;

	bool importData(const QString& fileName);
	bool exportData(const QString& fileName);

	QImage image;
	EnumContainerT<AnchorPosition> anchorPosition;
	EnumContainerT<SizeTarget> sizeTarget;
	BoolContainer fillUpper;
	BoolContainer fillLower;
	OpacityContainer opacity;

	std::vector<ValueSizePair> valueSizePairs;
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_SETTING_H
