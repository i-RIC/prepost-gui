#include "cgnsfileopener.h"
#include "private/cgnsfileopener_impl.h"

#include <cgnslib.h>

#include <stdexcept>

namespace {

std::string lock_filename(const std::string& filename)
{
	std::string l_filename = filename;
	l_filename.append(".lock");

	return l_filename;
}

} // namespace

CgnsFileOpener::Impl::Impl(const std::string &filename) :
	m_fileId {0},
	m_fileLocker {lock_filename(filename)}
{}

CgnsFileOpener::CgnsFileOpener(const std::string& filename, int openMode) :
	impl {new Impl {filename}}
{
	bool ok = impl->m_fileLocker.lock();
	if (! ok) {
		throw std::runtime_error("CGNS file open error");
	}
	int ier = cg_open(filename.c_str(), openMode, &(impl->m_fileId));
	if (ier != 0) {
		impl->m_fileLocker.unlock();
		throw std::runtime_error("CGNS file open error");
	}
}

CgnsFileOpener::~CgnsFileOpener()
{
	cg_close(impl->m_fileId);
	impl->m_fileLocker.unlock();

	delete impl;
}

int CgnsFileOpener::fileId() const
{
	return impl->m_fileId;
}
