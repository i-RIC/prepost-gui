#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindownodevectorarrowgroupstructureddataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog;
}

class Region2dSettingEditWidget;
class Structured2dFilteringSettingEditWidget;

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowNodeVectorArrowGroupStructuredDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	Post2dWindowNodeVectorArrowGroupStructuredDataItem* m_item;

	class AdditionalWidgets;
	AdditionalWidgets* m_additionalWidgets;

	Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog *ui;

};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_PROPERTYDIALOG_H
