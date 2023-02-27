#ifndef ARROWSLENGTHSETTINGEDITWIDGET_H
#define ARROWSLENGTHSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class ArrowsLengthSettingEditWidget;
}

class ArrowsSettingContainer;

class GUICOREDLL_EXPORT ArrowsLengthSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsLengthSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsLengthSettingEditWidget();

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

private:
	Ui::ArrowsLengthSettingEditWidget *ui;
};

#endif // ARROWSLENGTHSETTINGEDITWIDGET_H
