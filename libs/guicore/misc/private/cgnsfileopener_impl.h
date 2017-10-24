#ifndef CGNSFILEOPENER_IMPL_H
#define CGNSFILEOPENER_IMPL_H

#include "../cgnsfileopener.h"

#include <filelocker.h>

class CgnsFileOpener::Impl
{
public:
	Impl(const std::string& filename);

	int m_fileId;
	FileLocker m_fileLocker;
};

#endif // CGNSFILEOPENER_IMPL_H
