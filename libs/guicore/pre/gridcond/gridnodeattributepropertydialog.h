#ifndef GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
#define GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>
#include <guibase/contoursettingwidget.h>
#include "../../scalarstocolors/scalarstocolorseditdialog.h"

class ScalarsToColorsEditWidget;

namespace Ui
{
	class GridNodeAttributePropertyDialog;
}

class GUICOREDLL_EXPORT GridNodeAttributePropertyDialog : public QDialog
{
	Q_OBJECT

public:
	GridNodeAttributePropertyDialog(QWidget* parent = nullptr);
	~GridNodeAttributePropertyDialog();
	void setContour(ContourSettingWidget::Contour contour);
	ContourSettingWidget::Contour contour();
	int numberOfDivision();
	void setNumberOfDivision(int n);
	void setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget);
	void setOpacityPercent(int opacity);
	int opacityPercent();
	void accept() override;

private slots:
	void handleContourChange();

private:
	Ui::GridNodeAttributePropertyDialog* ui;
};

#endif // GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
