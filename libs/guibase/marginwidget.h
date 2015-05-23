#ifndef MARGINWIDGET_H
#define MARGINWIDGET_H

#include "guibase_global.h"

#include <QWidget>

/// Widget to add margin around a widget
class GUIBASEDLL_EXPORT MarginWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MarginWidget(QWidget* parent = nullptr);
	/// The margin (in pixels)
	int margin() const {return m_margin;}
	/// Set the margin (in pixels)
	void setMargin(int m) {m_margin = m;}
	/// Thw widget shown in the center
	QWidget* widget() const {return m_widget;}
	/// Set the widget shown in the center
	void setWidget(QWidget* w);

private:
	int m_margin;
	QWidget* m_widget;
};

#endif // MARGINWIDGET_H
