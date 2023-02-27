#ifndef ARROWSSHAPESETTINGEDITWIDGET_H
#define ARROWSSHAPESETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class ArrowsShapeSettingEditWidget;
}

class ArrowsSettingContainer;

class GUICOREDLL_EXPORT ArrowsShapeSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsShapeSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsShapeSettingEditWidget();

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

private:
	Ui::ArrowsShapeSettingEditWidget *ui;
};

#endif // ARROWSSHAPESETTINGEDITWIDGET_H
