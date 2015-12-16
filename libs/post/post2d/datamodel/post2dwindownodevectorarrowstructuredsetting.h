#ifndef POST2DWINDOWNODEVECTORARROWSTRUCTUREDSETTING_H
#define POST2DWINDOWNODEVECTORARROWSTRUCTUREDSETTING_H

#include "post2dwindownodevectorarrowsetting.h"

#include <guibase/structuredgridregion.h>
#include <misc/intcontainer.h>

class Post2dWindowNodeVectorArrowStructuredSetting : public Post2dWindowNodeVectorArrowSetting
{
public:
	Post2dWindowNodeVectorArrowStructuredSetting();
	Post2dWindowNodeVectorArrowStructuredSetting(const Post2dWindowNodeVectorArrowStructuredSetting& c);

	Post2dWindowNodeVectorArrowStructuredSetting& operator=(const Post2dWindowNodeVectorArrowStructuredSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	IntContainer iSampleRate;
	IntContainer jSampleRate;
	StructuredGridRegion::Range2d range;
};

#endif // POST2DWINDOWNODEVECTORARROWSTRUCTUREDSETTING_H
