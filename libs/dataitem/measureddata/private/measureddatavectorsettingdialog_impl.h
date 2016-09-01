#ifndef MEASUREDDATAVECTORSETTINGDIALOG_IMPL_H
#define MEASUREDDATAVECTORSETTINGDIALOG_IMPL_H

#include "../measureddatavectorsetting.h"
#include "../measureddatavectorsettingdialog.h"

class MeasuredDataVectorSettingDialog::Impl
{
public:
	Impl(MeasuredDataVectorSettingDialog* dialog);

	void setupSolutionComboBox(MeasuredData* data);

	MeasuredDataVectorSetting m_setting;

	std::vector<std::string> m_targets;
	std::vector<std::string> m_scalars;

private:
	MeasuredDataVectorSettingDialog* m_dialog;
};

#endif // MEASUREDDATAVECTORSETTINGDIALOG_IMPL_H
