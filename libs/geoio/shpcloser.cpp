#include "shpcloser.h"

SHPCloser::SHPCloser(SHPHandle handle) :
	m_handle {handle}
{}

SHPCloser::~SHPCloser()
{
	SHPClose(m_handle);
}
