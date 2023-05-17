#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_IMPL_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_IMPL_H

#include "post2dwindownodevectorparticlegroupunstructureddataitem_setting.h"
#include "../post2dwindownodevectorparticlegroupunstructureddataitem.h"

#include <guibase/vtktool/vtklineactor.h>

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Impl
{
public:
	Impl();

	Setting m_setting;

	bool m_dragging;
	vtkLineActor m_previewActor;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_IMPL_H
