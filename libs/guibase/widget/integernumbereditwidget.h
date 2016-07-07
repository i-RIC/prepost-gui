#ifndef INTEGERNUMBEREDITWIDGET_H
#define INTEGERNUMBEREDITWIDGET_H

#include "../guibase_global.h"

#include <QSpinBox>

/**
 * @brief Widget to edit integer number
 *
 * It is just a spinbox, but the default min and max values are changed.
 * It is because QSpinbox default min is 0, and max = 100, and those values
 * are not appropriate in many cases in iRIC.
 */
class GUIBASEDLL_EXPORT IntegerNumberEditWidget : public QSpinBox
{
	Q_OBJECT
public:
	IntegerNumberEditWidget(QWidget* parent);
};

#endif // INTEGERNUMBEREDITWIDGET_H
