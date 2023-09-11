#ifndef VTKGRAPHICSVIEWSCALEWIDGET_SCALEEDITDIALOG_H
#define VTKGRAPHICSVIEWSCALEWIDGET_SCALEEDITDIALOG_H

#include "../vtkgraphicsviewscalewidget.h"

#include <QDialog>

namespace Ui {
class VtkGraphicsViewScaleWidget_ScaleEditDialog;
}

class VtkGraphicsViewScaleWidget::ScaleEditDialog : public QDialog
{
	Q_OBJECT

public:
	ScaleEditDialog(QWidget *parent);
	~ScaleEditDialog();

	double scale() const;
	void setScale(double scale);

private:
	Ui::VtkGraphicsViewScaleWidget_ScaleEditDialog *ui;
};

#endif // VTKGRAPHICSVIEWSCALEWIDGET_SCALEEDITDIALOG_H
