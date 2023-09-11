#include "qlabelwithclickedsignal.h"

QLabelWithClickedSignal::QLabelWithClickedSignal(QWidget* parent) :
	QLabel {parent}
{}

void QLabelWithClickedSignal::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}
