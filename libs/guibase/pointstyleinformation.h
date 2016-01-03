#ifndef POINTSTYLEINFORMATION_H
#define POINTSTYLEINFORMATION_H

#include "guibase_global.h"
#include <Qt>
#include <QColor>

/// Container to handle point style information
class GUIBASEDLL_EXPORT PointStyleInformation
{

public:
	enum PointShape {
		ShapeCircle, ///< Circle
		ShapeBox     ///< Box (Square)
	};
	PointStyleInformation();

	/// Point size in pixels
	qreal size() const {return m_size;}
	/// Set point size in pixels
	void setSize(qreal size) {m_size = size;}
	/// Point color
	const QColor& color() const {return m_color;}
	/// Set point color
	void setColor(const QColor& c) {m_color = c;}
	/// Point shape (circle or box)
	PointShape pointShape() const {return m_pointShape;}
	/// Set point shape (circle or box)
	void setPointShape(PointShape shape) {m_pointShape = shape;}

private:
	qreal m_size;
	QColor m_color;
	PointShape m_pointShape;
};

#endif // POINTSTYLEINFORMATION_H
