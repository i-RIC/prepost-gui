#ifndef CGNSFILEOPENER_IMPL_H
#define CGNSFILEOPENER_IMPL_H

#include "../cgnsfileopener.h"

class CgnsFileOpener::Impl
{
public:
	Impl(const std::string& filename);

	int m_fileId;
};

#endif // CGNSFILEOPENER_IMPL_H
