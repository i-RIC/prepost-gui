#ifndef LOOKUPTABLEEDITWIDGET_H
#define LOOKUPTABLEEDITWIDGET_H

#include "../guicore_global.h"
#include "scalarstocolorseditwidget.h"
#include <guibase/contoursettingwidget.h>

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
	void setFillUpper(bool fill);
	void setFillLower(bool fill);
	void setDivisionNumber(int div);
	bool autoRange();
	double upperLimit();
	double lowerLimit();
	bool fillUpper();
	bool fillLower();
	int divisionNumber();
	void hideFillSettings();
	void hideDivisionNumber();
	void showDivisionNumber();
	void save() override;

public slots:
	void autoRangeUpdate(bool a);
	void setContourSetting(ContourSettingWidget::Contour contour);

protected:
	void setupWidget() override;

private:
	Ui::LookupTableEditWidget* ui;
};

#endif // LOOKUPTABLEEDITWIDGET_H
