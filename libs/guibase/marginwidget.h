#ifndef MARGINWIDGET_H
#define MARGINWIDGET_H

#include "guibase_global.h"

#include <QWidget>

class GUIBASEDLL_EXPORT MarginWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MarginWidget(QWidget *parent = nullptr);
	void setMargin(int m){m_margin = m;}
	void setWidget(QWidget* w);
	QWidget* widget() const {return m_widget;}

private:
	int m_margin;
	QWidget* m_widget;
};

#endif // MARGINWIDGET_H
