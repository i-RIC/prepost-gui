#include "cgnsfileopener.h"
#include "private/cgnsfileopener_impl.h"

#ifdef _MSC_VER
#include <hdf5.h>
#endif
#include <cgnslib.h>

#include <stdexcept>

CgnsFileOpener::Impl::Impl(const std::string &/*filename*/) :
	m_fileId {0}
{}

CgnsFileOpener::CgnsFileOpener(const std::string& filename, int openMode) :
	impl {new Impl {filename}}
{
	int ier = cg_open(filename.c_str(), openMode, &(impl->m_fileId));
	if (ier != 0) {
		H5close();
		throw std::runtime_error("CGNS file open error");
	}
}

CgnsFileOpener::~CgnsFileOpener()
{
	cg_close(impl->m_fileId);
#ifdef _MSC_VER
	// force linked files to close like Case1_Solution1.cgn, Case1_Solution2.cgn, ...
	herr_t err = H5close();
	Q_ASSERT(err == 0);
#endif
	delete impl;
}

int CgnsFileOpener::fileId() const
{
	return impl->m_fileId;
}
