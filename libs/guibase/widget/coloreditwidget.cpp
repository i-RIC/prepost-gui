#include "coloreditwidget.h"

#include <QColorDialog>

ColorEditWidget::ColorEditWidget(QWidget* parent) :
	QLabel(parent)
{
	setFrameShape(QFrame::WinPanel);
	setFrameShadow(QFrame::Sunken);
	setMinimumSize(45, 20);
	setMaximumSize(45, 20);
}

void ColorEditWidget::setColor(const QColor& c)
{
	m_color = c;
	QString r = QString::number(m_color.red());
	QString g = QString::number(m_color.green());
	QString b = QString::number(m_color.blue());
	QString str = QString("QLabel{background-color : rgb(%1, %2, %3);}")
								.arg(r).arg(g).arg(b);
	setStyleSheet(str);
}

QColor ColorEditWidget::color() const
{
	return m_color;
}

void ColorEditWidget::mousePressEvent(QMouseEvent* /*ev*/)
{
	QString ss = styleSheet();
	setStyleSheet("");
	QColor color = QColorDialog::getColor(this->color(), this);
	bool changed = false;
	if (color.isValid()) {
		changed = (m_color != color);
		setColor(color);
	} else {
		setStyleSheet(ss);
	}
	if (changed) {
		emit colorChanged(color);
	}
}

