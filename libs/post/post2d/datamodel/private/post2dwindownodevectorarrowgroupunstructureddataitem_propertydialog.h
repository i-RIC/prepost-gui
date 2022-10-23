#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindownodevectorarrowgroupunstructureddataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog;
}

class Region2dSettingEditWidget;
class GeneralFilteringSettingEditWidget;

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;

	class AdditionalWidgets;
	AdditionalWidgets* m_additionalWidgets;

	Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_PROPERTYDIALOG_H
