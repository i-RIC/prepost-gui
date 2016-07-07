#ifndef GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
#define GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H

#include "../../guicore_global.h"
#include "../../scalarstocolors/scalarstocolorseditdialog.h"

#include <guibase/widget/contoursettingwidget.h>

#include <QDialog>

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

	void setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget);

	ContourSettingWidget::Contour contour() const;
	void setContour(ContourSettingWidget::Contour contour);

	int numberOfDivision() const;
	void setNumberOfDivision(int n);

	int opacityPercent() const;
	void setOpacityPercent(int opacity);

	void accept() override;

private slots:
	void handleContourChange();

private:
	Ui::GridNodeAttributePropertyDialog* ui;
};

#endif // GRIDRELATEDNODECONDITIONPROPERTYDIALOG_H
