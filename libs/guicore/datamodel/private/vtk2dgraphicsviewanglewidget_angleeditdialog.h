#ifndef VTK2DGRAPHICSVIEWANGLEWIDGET_ANGLEEDITDIALOG_H
#define VTK2DGRAPHICSVIEWANGLEWIDGET_ANGLEEDITDIALOG_H

#include "../vtk2dgraphicsviewanglewidget.h"

#include <QDialog>

namespace Ui {
class Vtk2dGraphicsViewAngleWidget_AngleEditDialog;
}

class Vtk2dGraphicsViewAngleWidget::AngleEditDialog : public QDialog
{
	Q_OBJECT

public:
	AngleEditDialog(QWidget *parent);
	~AngleEditDialog();

	double angle() const;
	void setAngle(double angle);

private:
	Ui::Vtk2dGraphicsViewAngleWidget_AngleEditDialog *ui;
};

#endif // VTK2DGRAPHICSVIEWANGLEWIDGET_ANGLEEDITDIALOG_H
