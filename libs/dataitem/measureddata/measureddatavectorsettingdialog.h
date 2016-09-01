#ifndef MEASUREDDATAVECTORSETTINGDIALOG_H
#define MEASUREDDATAVECTORSETTINGDIALOG_H

#include <QDialog>

class MeasuredData;
class MeasuredDataVectorSetting;

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
	Ui::MeasuredDataVectorSettingDialog* ui;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/measureddatavectorsettingdialog_impl.h"
#endif // _DEBUG

#endif // MEASUREDDATAVECTORSETTINGDIALOG_H
