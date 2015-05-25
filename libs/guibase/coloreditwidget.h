#ifndef COLOREDITWIDGET_H
#define COLOREDITWIDGET_H

#include "guibase_global.h"

#include <QLabel>
/**
 * @brief Widget to edit color
 *
 * This widget look like a small box with color. When user click
 * on this, color edit dialog will be shown, and user can select
 * new color.
 */
class GUIBASEDLL_EXPORT ColorEditWidget : public QLabel
{
	Q_OBJECT

public:
	ColorEditWidget(QWidget* parent = nullptr);
	/// Set the color
	void setColor(const QColor& c);
	/// The color
	QColor color();

signals:
	void colorChanged(const QColor& c);

protected:
	void mousePressEvent(QMouseEvent* ev) override;
	QColor m_color;
};

#endif // COLOREDITWIDGET_H
