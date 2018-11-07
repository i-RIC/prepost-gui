#ifndef HYDRAULICDATA_IMPL_H
#define HYDRAULICDATA_IMPL_H

#include "../hydraulicdata.h"

#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class HydraulicData::Impl : public CompositeContainer
{
public:
	Impl();

	StringContainer m_name;
	StringContainer m_caption;
};

#endif // HYDRAULICDATA_IMPL_H
