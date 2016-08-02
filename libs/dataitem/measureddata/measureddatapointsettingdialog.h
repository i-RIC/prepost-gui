#ifndef MEASUREDDATAPOINTSETTINGDIALOG_H
#define MEASUREDDATAPOINTSETTINGDIALOG_H

#include "measureddatapointsetting.h"
#include <guibase/widget/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QDialog>

#include <unordered_map>
#include <vector>

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
	void showColorBarDialog();

private:
	Ui::MeasuredDataPointSettingDialog* ui;

	MeasuredDataPointSetting m_setting;

	std::unordered_map<std::string, QString> m_scalarBarTitleMap;
	std::unordered_map<std::string, LookupTableContainer*> m_lookupTables;
	std::vector<std::string> m_targets;

	LookupTableContainer m_lookupTable;
};

#endif // MEASUREDDATAPOINTSETTINGDIALOG_H
