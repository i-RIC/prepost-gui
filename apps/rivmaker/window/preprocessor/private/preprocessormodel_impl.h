#ifndef PREPROCESSORMODEL_IMPL_H
#define PREPROCESSORMODEL_IMPL_H

#include "../preprocessormodel.h"

class PreProcessorModel::Impl
{
public:
	Impl();
	~Impl();

	Project* m_project;
};

#endif // PREPROCESSORMODEL_IMPL_H
