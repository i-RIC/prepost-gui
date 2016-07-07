#include "mousepositionlabel.h"
#include <QVector2D>

MousePositionLabel::MousePositionLabel(QWidget* parent) :
	QLabel(parent)
{
	setFrameShape(QFrame::WinPanel);
	setFrameShadow(QFrame::Sunken);
	setMinimumWidth(100);
}

void MousePositionLabel::updatePosition(const QVector2D& position)
{
	setText(QString("X: %1, Y: %2").arg(position.x()).arg(position.y()));
}

void MousePositionLabel::clear()
{
	setText("X: ------ Y: ------");
}
