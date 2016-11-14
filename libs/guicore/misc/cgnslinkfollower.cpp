#include "cgnslinkfollower.h"
#include "private/cgnslinkfollower_impl.h"

#include <cgnslib.h>

CgnsLinkFollower::Impl::Impl() :
	m_fileId {0}
{
	int pathlen;
	int ier = cg_is_link(&pathlen);

	if (ier != 0) {return;}
	if (pathlen == 0) {return;}

	char* filename;
	char* path;

	cg_link_read(&filename, &path);

	std::string fullpath = "/";
	fullpath.append(path);

	ier = cg_open(filename, CG_MODE_READ, &m_fileId);
	if (ier != 0) {return;}

	cg_gopath(m_fileId, fullpath.c_str());
}

CgnsLinkFollower::Impl::~Impl()
{
	close();
}

void CgnsLinkFollower::Impl::close()
{
	if (m_fileId == 0) {return;}

	cg_close(m_fileId);
}

// public interfaces

CgnsLinkFollower::CgnsLinkFollower() :
	impl {new Impl()}
{}

CgnsLinkFollower::~CgnsLinkFollower()
{
	delete impl;
}

void CgnsLinkFollower::close()
{
	impl->close();
}
