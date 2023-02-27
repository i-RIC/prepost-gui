#ifndef GRIDBIRDEYEWINDOWGRIDDATAITEM_EDITDISPLAYSETTINGDIALOG_H
#define GRIDBIRDEYEWINDOWGRIDDATAITEM_EDITDISPLAYSETTINGDIALOG_H

#include "../gridbirdeyewindowgriddataitem.h"

#include <QDialog>

namespace Ui {
class GridBirdEyeWindowGridDataItem_EditDisplaySettingDialog;
}

class GridBirdEyeWindowSetting;
class SolverDefinitionGridType;

class QAbstractButton;

class GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EditDisplaySettingDialog(GridBirdEyeWindowGridDataItem* item, QWidget *parent = nullptr);
	~EditDisplaySettingDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void colorTargetChanged(int index);
	void handleButtonClick(QAbstractButton* button);

private:
	GridBirdEyeWindowSetting setting() const;
	void setSetting(const GridBirdEyeWindowSetting& setting);
	void apply();

	void setupWarpTargets();
	void setupColorTargets();
	std::string colorTarget() const;

	bool m_applied;

	std::vector<std::string> m_warpTargets;
	std::vector<std::string> m_colorTargets;

	SolverDefinitionGridType* m_gridType;
	GridBirdEyeWindowGridDataItem* m_item;

	Ui::GridBirdEyeWindowGridDataItem_EditDisplaySettingDialog *ui;
};

#endif // GRIDBIRDEYEWINDOWGRIDDATAITEM_EDITDISPLAYSETTINGDIALOG_H
