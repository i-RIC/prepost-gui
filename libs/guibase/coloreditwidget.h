#ifndef COLOREDITWIDGET_H
#define COLOREDITWIDGET_H

#include "guibase_global.h"

#include <QLabel>

class GUIBASEDLL_EXPORT ColorEditWidget : public QLabel
{
	Q_OBJECT

public:
	ColorEditWidget(QWidget* parent = nullptr);
	void setColor(const QColor& c);
	QColor color();

signals:
	void colorChanged(const QColor& c);

protected:
	void mousePressEvent(QMouseEvent* ev);
	QColor m_color;
};

#endif // COLOREDITWIDGET_H
