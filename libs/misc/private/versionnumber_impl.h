#ifndef VERSIONNUMBER_IMPL_H
#define VERSIONNUMBER_IMPL_H

#include "../versionnumber.h"

class VersionNumber::Impl
{
public:
	unsigned int m_major {};
	unsigned int m_minor {};
	unsigned int m_fix {};
	unsigned int m_build {};
};

#endif // VERSIONNUMBER_IMPL_H
