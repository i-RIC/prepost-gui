#ifndef INTEGERNUMBEREDITWIDGET_H
#define INTEGERNUMBEREDITWIDGET_H

#include "guibase_global.h"

#include <QSpinBox>

class GUIBASEDLL_EXPORT IntegerNumberEditWidget : public QSpinBox
{
	Q_OBJECT
public:
	IntegerNumberEditWidget(QWidget* parent);
};

#endif // INTEGERNUMBEREDITWIDGET_H
