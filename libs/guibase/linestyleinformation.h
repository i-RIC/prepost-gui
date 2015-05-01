#ifndef LINESTYLEINFORMATION_H
#define LINESTYLEINFORMATION_H

#include "guibase_global.h"
#include <Qt>
#include <QColor>

class GUIBASEDLL_EXPORT LineStyleInformation
{
public:
	/// Constructor
	LineStyleInformation();
	qreal width() const {return m_width;}
	void setWidth(qreal w) {m_width = w;}
	const QColor& color() const {return m_color;}
	void setColor(const QColor& c) {m_color = c;}
	Qt::PenStyle penStyle() const {return m_penStyle;}
	void setPenStyle(Qt::PenStyle ps) {m_penStyle = ps;}

private:
	qreal m_width;
	QColor m_color;
	Qt::PenStyle m_penStyle;
};

#endif // LINESTYLEINFORMATION_H
