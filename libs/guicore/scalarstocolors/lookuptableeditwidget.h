#ifndef LOOKUPTABLEEDITWIDGET_H
#define LOOKUPTABLEEDITWIDGET_H

#include "../guicore_global.h"
#include "scalarstocolorseditwidget.h"

#include <guibase/widget/contoursettingwidget.h>

namespace Ui
{
	class LookupTableEditWidget;
}

class LookupTableContainer;

class GUICOREDLL_EXPORT LookupTableEditWidget : public ScalarsToColorsEditWidget
{
	Q_OBJECT

public:
	LookupTableEditWidget(QWidget* parent);
	~LookupTableEditWidget();

	void hideFillSettings();
	void hideDivisionNumber();
	void showDivisionNumber();

	bool fillUpper();
	void setFillUpper(bool fill);

	bool fillLower();
	void setFillLower(bool fill);

	int divisionNumber();
	void setDivisionNumber(int div);

	bool autoRange();
	double upperLimit();
	double lowerLimit();

	void save() override;

public slots:
	void autoRangeUpdate(bool a);
	void setContourSetting(ContourSettingWidget::Contour contour);

private:
	void setupWidget() override;

private:
	Ui::LookupTableEditWidget* ui;
};

#endif // LOOKUPTABLEEDITWIDGET_H
