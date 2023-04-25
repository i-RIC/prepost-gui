#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindownodevectorarrowdataitem.h"
#include "../post3dwindownodevectorarrowgroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post3dWindowNodeVectorArrowGroupDataItem_PropertyDialog;
}

class Post3dWindowNodeVectorArrowGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post3dWindowNodeVectorArrowGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void addFace();
	void removeFace();
	void setCurrentFace(int row);
	void updateImageSetting();

	void handleButtonClick(QAbstractButton* button);

private:
	void updateFaceList();
	void saveCurrentFace();
	void clearWidgets();

	void apply();

	ArrowsSettingContainer m_setting;
	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> m_settings;
	std::unordered_map<std::string, ColorMapSettingContainer> m_colorMapSettings;
	Post3dWindowNodeVectorArrowGroupDataItem* m_item;

	int m_currentRow;
	int m_gridDimensions[3];
	bool m_applied;

	Ui::Post3dWindowNodeVectorArrowGroupDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_PROPERTYDIALOG_H
