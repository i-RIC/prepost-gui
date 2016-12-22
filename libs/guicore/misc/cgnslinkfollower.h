#ifndef CGNSLINKFOLLOWER_H
#define CGNSLINKFOLLOWER_H

#include "../guicore_global.h"

class GUICOREDLL_EXPORT CgnsLinkFollower
{
public:
	CgnsLinkFollower();
	~CgnsLinkFollower();

	void close();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/cgnslinkfollower_impl.h"
#endif // _DEBUG

#endif // CGNSLINKFOLLOWER_H
