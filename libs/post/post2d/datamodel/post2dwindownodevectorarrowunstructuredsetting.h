#ifndef POST2DWINDOWNODEVECTORARROWUNSTRUCTUREDSETTING_H
#define POST2DWINDOWNODEVECTORARROWUNSTRUCTUREDSETTING_H

#include "post2dwindownodevectorarrowsetting.h"

class Post2dWindowNodeVectorArrowUnstructuredSetting : public Post2dWindowNodeVectorArrowSetting
{
public:
	Post2dWindowNodeVectorArrowUnstructuredSetting();
	Post2dWindowNodeVectorArrowUnstructuredSetting(const Post2dWindowNodeVectorArrowUnstructuredSetting& c);

	Post2dWindowNodeVectorArrowUnstructuredSetting& operator=(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;
};

#endif // POST2DWINDOWNODEVECTORARROWUNSTRUCTUREDSETTING_H
