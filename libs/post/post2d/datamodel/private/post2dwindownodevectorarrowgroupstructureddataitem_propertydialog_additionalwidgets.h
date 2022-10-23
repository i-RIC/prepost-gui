#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H

#include "post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.h"

#include <QWidget>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog_AdditionalWidgets;
}

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets : public QWidget
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
	Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog_AdditionalWidgets *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_ADDITIONALWIDGETS_H
