#include "qfontdisplaylabel.h"

#include <QFont>

QFontDisplayLabel::QFontDisplayLabel(QWidget* parent) :
	QLabel {parent}
{}

void QFontDisplayLabel::setFont(const QFont& font)
{
	auto label = font.family();
	if (font.pixelSize() != -1) {
		label += QString(" %1 px").arg(font.pixelSize());
	} else if (font.pointSize() != -1) {
		label += QString(" %1 pt").arg(font.pointSize());
	}
	setText(label);
	QLabel::setFont(font);
}
