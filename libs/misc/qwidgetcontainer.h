#ifndef QWIDGETCONTAINER_H
#define QWIDGETCONTAINER_H

#include "misc_global.h"

#include <QWidget>

class MISCDLL_EXPORT QWidgetContainer : public QWidget
{
public:
	QWidgetContainer(QWidget* parent = nullptr);

	QWidget* widget() const;
	void setWidget(QWidget* widget);

	QSize sizeHint() const override;

private:
	QWidget* m_widget;
};

#endif // QWIDGETCONTAINER_H
