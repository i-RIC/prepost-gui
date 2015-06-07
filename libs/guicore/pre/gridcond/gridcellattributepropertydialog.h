#ifndef GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H
#define GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>
#include "../../scalarstocolors/scalarstocolorseditdialog.h"

class ScalarsToColorsEditWidget;

namespace Ui
{
	class GridCellAttributePropertyDialog;
}

class GUICOREDLL_EXPORT GridCellAttributePropertyDialog : public QDialog
{
	Q_OBJECT
public:
	GridCellAttributePropertyDialog(QWidget* parent = nullptr);
	~GridCellAttributePropertyDialog();
	void setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget);
	ScalarsToColorsEditWidget* scalarsToColorsEditWidget() const;
	void setOpacityPercent(int opacity);
	int opacityPercent();
	void accept() override;

private:
	Ui::GridCellAttributePropertyDialog* ui;
};

#endif // GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H
