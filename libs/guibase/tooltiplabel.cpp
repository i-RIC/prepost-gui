#include "tooltiplabel.h"

#include <QMessageBox>

TooltipLabel::TooltipLabel(QWidget* parent) :
	QLabel(parent)
{
	setPixmap(QPixmap(":/libs/guibase/images/iconQuestion.png"));
}

void TooltipLabel::mousePressEvent(QMouseEvent* /*event*/)
{
	QMessageBox::information(this, tr("Help"), toolTip());
}
