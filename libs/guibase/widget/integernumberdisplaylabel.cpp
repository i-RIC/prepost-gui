#include "integernumberdisplaylabel.h"

IntegerNumberDisplayLabel::IntegerNumberDisplayLabel(QWidget* parent) :
	QLabel(parent)
{
	setFrameShape(QFrame::WinPanel);
	setFrameShadow(QFrame::Sunken);
	setMinimumSize(100, 19);
	setAlignment(Qt::AlignRight);

	m_clearMode = cmLines;
	clear();
}

IntegerNumberDisplayLabel::ClearMode IntegerNumberDisplayLabel::clearMode() const
{
	return m_clearMode;
}

void IntegerNumberDisplayLabel::setClearMode(ClearMode cm)
{
	m_clearMode = cm;
}

void IntegerNumberDisplayLabel::setValue(unsigned int value)
{
	setText(QString("%1").arg(value));
}

void IntegerNumberDisplayLabel::clear()
{
	if (m_clearMode == cmLines) {
		setText(tr("------"));
	} else {
		setText("");
	}
}
