#ifndef LOGODATAITEM_IMPL_H
#define LOGODATAITEM_IMPL_H

#include "../logodataitem.h"
#include "../logosetting.h"

class vtkActor2D;

class LogoDataItem::Impl
{
public:
	Impl();
	~Impl();

	LogoSetting m_setting;

	vtkActor2D* m_actor;
};

#endif // LOGODATAITEM_IMPL_H
