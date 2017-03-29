#ifndef VALUEWIDGET_H
#define VALUEWIDGET_H

#include <QWidget>

namespace Ui {
class ValueWidget;
}

class ValueWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ValueWidget(QWidget *parent = nullptr);
	~ValueWidget();

public slots:
	void setValue(double val);
	void clear();

private:
	Ui::ValueWidget *ui;
};

#endif // VALUEWIDGET_H
