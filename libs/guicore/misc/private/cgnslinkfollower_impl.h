#ifndef CGNSLINKFOLLOWER_IMPL_H
#define CGNSLINKFOLLOWER_IMPL_H

#include "../cgnslinkfollower.h"

#include <string>

class CgnsLinkFollower::Impl
{
public:
	Impl();
	~Impl();

	void close();

	int m_fileId;
};

#endif // CGNSLINKFOLLOWER_IMPL_H
