#ifndef MEASUREDDATAPOINTGROUPDATAITEM_PROPERTYDIALOG_H
#define MEASUREDDATAPOINTGROUPDATAITEM_PROPERTYDIALOG_H

#include "../measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <QDialog>

#include <unordered_map>

namespace Ui {
class MeasuredDataPointGroupDataItem_PropertyDialog;
}

class ColorMapSettingContainer;
class MeasuredData;
class MeasuredDataPointSetting;

class QAbstractButton;
class QUndoCommand;

class MeasuredDataPointGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(MeasuredDataPointGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand();

	void accept() override;
	void reject() override;

private slots:
	void targetChanged(int index);
	void handleButtonClick(QAbstractButton* button);

private:
	MeasuredDataPointSetting setting() const;
	void setSetting(const MeasuredDataPointSetting& setting);
	void apply();

	bool m_applied;

	std::vector<std::string> m_targets;

	MeasuredDataPointGroupDataItem* m_item;

	Ui::MeasuredDataPointGroupDataItem_PropertyDialog *ui;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_PROPERTYDIALOG_H
