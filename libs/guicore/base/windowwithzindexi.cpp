#include "windowwithzindexi.h"

WindowWithZIndexI::WindowWithZIndexI() :
	m_zindex {0}
{}

unsigned int WindowWithZIndexI::zindex() const
{
	return m_zindex;
}

void WindowWithZIndexI::setZindex(unsigned int i)
{
	m_zindex = i;
}
