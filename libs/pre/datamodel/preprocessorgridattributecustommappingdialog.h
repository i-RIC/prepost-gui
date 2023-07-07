#ifndef PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H
#define PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H

#include "preprocessorcustommappingsetting.h"

#include <QDialog>
#include <QList>

#include <map>

class PreProcessorGridAttributeMappingSettingDataItem;
class PreProcessorBCSettingDataItem;

class QCheckBox;

namespace Ui
{
	class PreProcessorGridAttributeCustomMappingDialog;
}

class PreProcessorGridAttributeCustomMappingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreProcessorGridAttributeCustomMappingDialog(QWidget* parent = nullptr);
	~PreProcessorGridAttributeCustomMappingDialog();
	void setSettings(const QList<PreProcessorGridAttributeMappingSettingDataItem*>& atts, const QList<PreProcessorBCSettingDataItem*>& bcs, const PreProcessorCustomMappingSetting& setting);
	PreProcessorCustomMappingSetting setting() const;

private slots:
	void checkAll();
	void uncheckAll();

private:
	std::map<PreProcessorGridAttributeMappingSettingDataItem*, QCheckBox*> m_attMap;
	std::map<PreProcessorBCSettingDataItem*, QCheckBox*> m_bcMap;

	Ui::PreProcessorGridAttributeCustomMappingDialog* ui;
};

#endif // PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H
