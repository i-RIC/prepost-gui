#ifndef POSTSTRINGRESULTSETTINGCONTAINER_IMAGEBUILDER_H
#define POSTSTRINGRESULTSETTINGCONTAINER_IMAGEBUILDER_H

#include "../poststringresultengine.h"
#include "../poststringresultsettingcontainer.h"

#include <guicore/image/public/imagesettingcontainer_imagebuilder.h>

class v4PostZoneDataContainer;

class PostStringResultSettingContainer::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(PostStringResultSettingContainer* setting);

	bool build(QImage* image) override;
	QSize autoSize() const override;
	void setZoneDataContainer(v4PostZoneDataContainer* c);

private:
	PostStringResultSettingContainer* m_setting;

	PostStringResultEngine m_engine;
	v4PostZoneDataContainer* m_zoneDataContainer;
};

#endif // POSTSTRINGRESULTSETTINGCONTAINER_IMAGEBUILDER_H
