#include "realnumberdisplaylabel.h"

RealNumberDisplayLabel::RealNumberDisplayLabel(QWidget* parent) :
	QLabel(parent)
{
	setFrameShape(QFrame::WinPanel);
	setFrameShadow(QFrame::Sunken);
	setMinimumSize(100, 19);
	setAlignment(Qt::AlignRight);

	m_clearMode = cmLines;
	clear();
}

void RealNumberDisplayLabel::setValue(double value)
{
	setText(QString("%1").arg(value));
}

void RealNumberDisplayLabel::clear()
{
	if (m_clearMode == cmLines) {
		setText(tr("------"));
	} else {
		setText("");
	}
}
