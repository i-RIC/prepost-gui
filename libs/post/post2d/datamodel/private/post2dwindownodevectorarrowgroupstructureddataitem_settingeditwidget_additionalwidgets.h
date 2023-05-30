#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H

#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget_AdditionalWidgets;
}

class Region2dSettingEditWidget;
class Structured2dFilteringSettingEditWidget;

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets : public QWidget
{
	Q_OBJECT

public:
	explicit AdditionalWidgets(QWidget *parent = nullptr);
	~AdditionalWidgets();

	Structured2dFilteringSettingEditWidget* samplingWidget() const;
	Region2dSettingEditWidget* regionWidget() const;

private slots:
	void handleRegionModeChange();

private:
	Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget_AdditionalWidgets *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H
