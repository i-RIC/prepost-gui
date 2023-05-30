#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H

#include "post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget_AdditionalWidgets;
}

class GeneralFilteringSettingEditWidget;
class Region2dSettingEditWidget;

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::AdditionalWidgets : public QWidget
{
	Q_OBJECT

public:
	explicit AdditionalWidgets(QWidget *parent = nullptr);
	~AdditionalWidgets();

	GeneralFilteringSettingEditWidget* samplingWidget() const;
	Region2dSettingEditWidget* regionWidget() const;

private:
	Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget_AdditionalWidgets *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_ADDITIONALWIDGETS_H
