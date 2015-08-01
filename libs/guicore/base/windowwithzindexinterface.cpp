#include "windowwithzindexinterface.h"

WindowWithZIndexInterface::WindowWithZIndexInterface() :
	m_zindex {0}
{}

unsigned int WindowWithZIndexInterface::zindex() const
{
	return m_zindex;
}

void WindowWithZIndexInterface::setZindex(unsigned int i)
{
	m_zindex = i;
}
