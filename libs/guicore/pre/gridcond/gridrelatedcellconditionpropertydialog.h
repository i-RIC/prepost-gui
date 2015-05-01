#ifndef GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H
#define GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>
#include "../../scalarstocolors/scalarstocolorseditdialog.h"

class ScalarsToColorsEditWidget;

namespace Ui
{
	class GridRelatedCellConditionPropertyDialog;
}

class GUICOREDLL_EXPORT GridRelatedCellConditionPropertyDialog : public QDialog
{
	Q_OBJECT
public:
	GridRelatedCellConditionPropertyDialog(QWidget* parent = nullptr);
	~GridRelatedCellConditionPropertyDialog();
	void setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget);
	ScalarsToColorsEditWidget* scalarsToColorsEditWidget() const;
	void setOpacityPercent(int opacity);
	int opacityPercent();
	void accept();

private:
	Ui::GridRelatedCellConditionPropertyDialog* ui;
};

#endif // GRIDRELATEDCELLCONDITIONPROPERTYDIALOG_H
