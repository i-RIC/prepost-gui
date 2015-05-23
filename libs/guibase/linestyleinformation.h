#ifndef LINESTYLEINFORMATION_H
#define LINESTYLEINFORMATION_H

#include "guibase_global.h"
#include <Qt>
#include <QColor>

/// Container to handle line style information
class GUIBASEDLL_EXPORT LineStyleInformation
{
public:
	/// Constructor
	LineStyleInformation();
	/// Line width
	qreal width() const {return m_width;}
	/// Set line width
	void setWidth(qreal w) {m_width = w;}
	/// Line color
	const QColor& color() const {return m_color;}
	/// Set line color
	void setColor(const QColor& c) {m_color = c;}
	/// Pen style
	Qt::PenStyle penStyle() const {return m_penStyle;}
	/// Set pen style
	void setPenStyle(Qt::PenStyle ps) {m_penStyle = ps;}

private:
	qreal m_width;
	QColor m_color;
	Qt::PenStyle m_penStyle;
};

#endif // LINESTYLEINFORMATION_H
