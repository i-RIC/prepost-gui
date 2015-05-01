#ifndef PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H
#define PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H

#include "preprocessorcustommappingsetting.h"

#include <QDialog>
#include <QList>
#include <QMap>

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

private:
	QMap<PreProcessorGridAttributeMappingSettingDataItem*, QCheckBox*> m_attMap;
	QMap<PreProcessorBCSettingDataItem*, QCheckBox*> m_bcMap;

	Ui::PreProcessorGridAttributeCustomMappingDialog* ui;
};

#endif // PREPROCESSORGRIDATTRIBUTECUSTOMMAPPINGDIALOG_H
