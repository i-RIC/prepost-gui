#ifndef COLORSOURCE_IMPL_H
#define COLORSOURCE_IMPL_H

#include <QColor>

#include <vector>

#include "../colorsource.h"

class ColorSource::Impl
{
public:
	std::vector<QColor> m_colors;
};

#endif // COLORSOURCE_IMPL_H
