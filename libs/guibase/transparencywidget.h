#ifndef TRANSPARENCYWIDGET_H
#define TRANSPARENCYWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui {
	class TransparencyWidget;
}

class GUIBASEDLL_EXPORT TransparencyWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TransparencyWidget(QWidget *parent = nullptr);
	~TransparencyWidget();
	void setOpacity(int opacity);
	int opacity();

private slots:
	void handleCheck(bool checked);

private:
	Ui::TransparencyWidget *ui;
};

#endif // TRANSPARENCYWIDGET_H
