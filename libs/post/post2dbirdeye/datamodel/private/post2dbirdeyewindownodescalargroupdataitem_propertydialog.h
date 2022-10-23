#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dbirdeyewindownodescalargroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post2dBirdEyeWindowNodeScalarGroupDataItem_PropertyDialog;
}

class Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);
	void handleColorScalarChange(int index);

private:
	Setting setting() const;
	void setSetting(const Setting& setting);

	void apply();

	bool m_applied;

	std::vector<std::string> m_colorMapNames;

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;

	Ui::Post2dBirdEyeWindowNodeScalarGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H
