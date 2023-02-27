#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H

#include "post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog_AdditionalWidgets;
}

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::AdditionalWidgets : public QWidget
{
	Q_OBJECT

public:
	explicit AdditionalWidgets(QWidget *parent = nullptr);
	~AdditionalWidgets();

	GeneralFilteringSettingEditWidget* samplingWidget() const;
	Region2dSettingEditWidget* regionWidget() const;

private:
	Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog_AdditionalWidgets *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H
