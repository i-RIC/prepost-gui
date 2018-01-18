#ifndef POST2DWINDOWNODEVECTORARROWSETTING_H
#define POST2DWINDOWNODEVECTORARROWSETTING_H

#include <guibase/arrowsettingcontainer.h>
#include <guibase/structuredgridregion.h>
#include <misc/enumcontainert.h>

class Post2dWindowNodeVectorArrowSetting : public ArrowSettingContainer
{
public:
	Post2dWindowNodeVectorArrowSetting();
	Post2dWindowNodeVectorArrowSetting(const Post2dWindowNodeVectorArrowSetting& setting);
	virtual ~Post2dWindowNodeVectorArrowSetting();

	Post2dWindowNodeVectorArrowSetting& operator=(const Post2dWindowNodeVectorArrowSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
};

#endif // POST2DWINDOWNODEVECTORARROWSETTING_H
