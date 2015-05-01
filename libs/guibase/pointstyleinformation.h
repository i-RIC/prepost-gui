#ifndef POINTSTYLEINFORMATION_H
#define POINTSTYLEINFORMATION_H

#include "guibase_global.h"
#include <Qt>
#include <QColor>

class GUIBASEDLL_EXPORT PointStyleInformation
{
public:
	enum PointShape {
		ShapeCircle, ///< Circle
		ShapeBox     ///< Box (Square)
	};
	/// Constructor
	PointStyleInformation();
	/// Point size in pixels on the screen
	qreal size() const {return m_size;}
	QColor color() const {return m_color;}
	PointShape pointShape() const {return m_pointShape;}

private:
	qreal m_size;
	QColor m_color;
	PointShape m_pointShape;
};

#endif // POINTSTYLEINFORMATION_H
