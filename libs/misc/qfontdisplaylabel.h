#ifndef QFONTDISPLAYLABEL_H
#define QFONTDISPLAYLABEL_H

#include <misc_global.h>

#include <QLabel>

class MISCDLL_EXPORT QFontDisplayLabel : public QLabel
{
public:
	QFontDisplayLabel(QWidget* parent = nullptr);

	void setFont(const QFont& font);
};

#endif // QFONTDISPLAYLABEL_H
