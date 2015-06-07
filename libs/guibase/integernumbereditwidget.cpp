#include "integernumbereditwidget.h"

IntegerNumberEditWidget::IntegerNumberEditWidget(QWidget* parent)
	: QSpinBox(parent)
{
	// Set minimum and maximum to the range supported by 32 bit integer.
	setMinimum(- 2147483646);
	setMaximum(2147483647);
}
