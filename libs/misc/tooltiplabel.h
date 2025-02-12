#ifndef TOOLTIPLABEL_H
#define TOOLTIPLABEL_H

#include "misc_global.h"

#include <QLabel>

class MISCDLL_EXPORT TooltipLabel : public QLabel
{
	Q_OBJECT

public:
	TooltipLabel(QWidget* parent);

protected:
	void mousePressEvent(QMouseEvent* event);
};

#endif // TOOLTIPLABEL_H
