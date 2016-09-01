#ifndef MEASUREDDATAPOINTSETTINGDIALOG_H
#define MEASUREDDATAPOINTSETTINGDIALOG_H

#include "measureddatapointsetting.h"

#include <QDialog>

#include <unordered_map>

namespace Ui
{
	class MeasuredDataPointSettingDialog;
}

class LookupTableContainer;
class MeasuredData;

class MeasuredDataPointSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MeasuredDataPointSettingDialog(QWidget* parent = nullptr);
	~MeasuredDataPointSettingDialog();

	void setData(MeasuredData* md);
	void setNoPolyData(bool noPolyData);
	void forceSelectPointsOnly();

	MeasuredDataPointSetting setting() const;
	void setSetting(const MeasuredDataPointSetting& setting);

	QString scalarBarTitle() const;
	void setScalarBarTitleMap(const std::unordered_map<std::string, QString>& titleMap);

	LookupTableContainer lookupTable() const;
	void setLookupTables(const std::unordered_map<std::string, LookupTableContainer*>& lookupTables);

public slots:
	void accept() override;

private slots:
	void targetChanged(int index);
	void handleContourChange(ContourSettingWidget::Contour c);
	void showColorBarDialog();

private:
	Ui::MeasuredDataPointSettingDialog* ui;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/measureddatapointsettingdialog_impl.h"
#endif // _DEBUG

#endif // MEASUREDDATAPOINTSETTINGDIALOG_H
