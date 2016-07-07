#ifndef MARGINWIDGET_H
#define MARGINWIDGET_H

#include "../guibase_global.h"

#include <QWidget>

/// Widget to add margin around a widget
class GUIBASEDLL_EXPORT MarginWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MarginWidget(QWidget* parent = nullptr);

	int marginPixels() const;
	void setMarginPixels(int m);

	QWidget* widget() const;
	void setWidget(QWidget* w);

private:
	int m_marginPixels;
	QWidget* m_widget;
};

#endif // MARGINWIDGET_H
