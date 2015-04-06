#ifndef GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
#define GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>
#include <guibase/contoursettingwidget.h>
#include "../../scalarstocolors/scalarstocolorseditdialog.h"

class ScalarsToColorsEditWidget;

namespace Ui {
	class GridRelatedNodeConditionPropertyDialog;
}

class GUICOREDLL_EXPORT GridRelatedNodeConditionPropertyDialog : public QDialog
{
	Q_OBJECT
public:
	GridRelatedNodeConditionPropertyDialog(QWidget *parent = 0);
	~GridRelatedNodeConditionPropertyDialog();
	void setContour(ContourSettingWidget::Contour contour);
	ContourSettingWidget::Contour contour();
	int numberOfDivision();
	void setNumberOfDivision(int n);
	void setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget);
	void setOpacityPercent(int opacity);
	int opacityPercent();
	void accept();

private slots:
	void handleContourChange();

private:
	Ui::GridRelatedNodeConditionPropertyDialog *ui;
};

#endif // GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
