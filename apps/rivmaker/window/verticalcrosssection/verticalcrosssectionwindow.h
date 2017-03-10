#ifndef VERTICALCROSSSECTIONWINDOW_H
#define VERTICALCROSSSECTIONWINDOW_H

#include <QWidget>

namespace Ui {
class VerticalCrossSectionWindow;
}

class VerticalCrossSectionWindow : public QWidget
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindow(QWidget *parent = 0);
	~VerticalCrossSectionWindow();

private:
	Ui::VerticalCrossSectionWindow *ui;
};

#endif // VERTICALCROSSSECTIONWINDOW_H
