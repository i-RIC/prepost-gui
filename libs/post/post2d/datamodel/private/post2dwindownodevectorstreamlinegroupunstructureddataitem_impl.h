#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_IMPL_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_IMPL_H

#include "post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.h"
#include "../post2dwindownodevectorstreamlinegroupunstructureddataitem.h"

#include <guibase/vtktool/vtklineactor.h>

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Impl
{
public:
	Impl();

	Setting m_setting;

	bool m_dragging;
	vtkLineActor m_previewActor;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_IMPL_H
