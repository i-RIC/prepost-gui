#ifndef POSTBASEITERATIVEVALUESCONTAINER_IMPL_H
#define POSTBASEITERATIVEVALUESCONTAINER_IMPL_H

#include "../postbaseiterativevaluescontainer.h"

class PostBaseIterativeValuesContainer::Impl
{
public:
	Impl();
	~Impl();

	std::vector<BaseContainer*> m_baseContainers;
	IO* m_io;
	ProjectData* m_projectData;
};


#endif // POSTBASEITERATIVEVALUESCONTAINER_IMPL_H
