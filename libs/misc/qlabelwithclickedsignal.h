#ifndef QLABELWITHCLICKEDSIGNAL_H
#define QLABELWITHCLICKEDSIGNAL_H

#include "misc_global.h"

#include <QLabel>

class MISCDLL_EXPORT QLabelWithClickedSignal : public QLabel
{
	Q_OBJECT

public:
	QLabelWithClickedSignal(QWidget* parent);

	void mousePressEvent(QMouseEvent *event) override;

signals:
	void clicked();
};

#endif // QLABELWITHCLICKEDSIGNAL_H
