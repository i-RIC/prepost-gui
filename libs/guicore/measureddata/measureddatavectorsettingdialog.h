#ifndef MEASUREDDATAVECTORSETTINGDIALOG_H
#define MEASUREDDATAVECTORSETTINGDIALOG_H

#include <QDialog>

#include "../project/measureddata.h"
#include "measureddatavectorgroupdataitem.h"
#include "measureddatavectorsetting.h"

#include <string>
#include <vector>

namespace Ui
{
	class MeasuredDataVectorSettingDialog;
}

class MeasuredDataVectorSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MeasuredDataVectorSettingDialog(QWidget* parent = nullptr);
	~MeasuredDataVectorSettingDialog();

	void setData(MeasuredData* data);

	MeasuredDataVectorSetting setting() const;
	void setSetting(const MeasuredDataVectorSetting& setting);

private:
	void setupSolutionComboBox(MeasuredData* data);

	MeasuredDataVectorSetting m_setting;

	std::vector<std::string> m_targets;
	std::vector<std::string> m_scalars;

	Ui::MeasuredDataVectorSettingDialog* ui;
};

#endif // MEASUREDDATAVECTORSETTINGDIALOG_H
