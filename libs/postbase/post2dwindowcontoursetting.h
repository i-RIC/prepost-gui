#ifndef POST2DWINDOWCONTOURSETTING_H
#define POST2DWINDOWCONTOURSETTING_H

#include "postbase_global.h"

#include <guibase/structuredgridregion.h>
#include <guibase/scalarsettingcontainer.h>
#include <misc/enumcontainert.h>

class POSTBASEDLL_EXPORT Post2dWindowContourSetting : public ScalarSettingContainer
{

public:
	Post2dWindowContourSetting();
	Post2dWindowContourSetting(const Post2dWindowContourSetting& s);
	~Post2dWindowContourSetting();

	Post2dWindowContourSetting& operator=(const Post2dWindowContourSetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s) override;

	// Region setting
	EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	StructuredGridRegion::Range2d range;
};


#endif // POST2DWINDOWCONTOURSETTING_H
