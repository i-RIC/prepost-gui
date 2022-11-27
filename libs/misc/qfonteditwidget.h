#ifndef QFONTEDITWIDGET_H
#define QFONTEDITWIDGET_H

#include "misc_global.h"

#include <QFont>
#include <QWidget>

namespace Ui {
class QFontEditWidget;
}

class MISCDLL_EXPORT QFontEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QFontEditWidget(QWidget *parent = nullptr);
	~QFontEditWidget();

	QFont font() const;
	void setFont(const QFont& font);

private slots:
	void openEditDialog();

private:
	QFont m_font;
	Ui::QFontEditWidget *ui;
};

#endif // QFONTEDITWIDGET_H
