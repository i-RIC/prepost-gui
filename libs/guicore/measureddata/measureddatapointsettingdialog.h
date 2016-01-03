#ifndef MEASUREDDATAPOINTSETTINGDIALOG_H
#define MEASUREDDATAPOINTSETTINGDIALOG_H

#include "../scalarstocolors/lookuptablecontainer.h"
#include "measureddatapointgroupdataitem.h"
#include <guibase/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <QDialog>
#include <QList>
#include <QMap>

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
	void setSetting(const MeasuredDataPointGroupDataItem::Setting& setting);
	void setScalarBarTitleMap(const QMap<QString, QString>& titlemap);
	void setLookupTables(const QMap<QString, LookupTableContainer*>& lookuptables);

	void forceSelectPointsOnly();

	MeasuredDataPointGroupDataItem::Setting setting() const;
	LookupTableContainer lookupTable() const;
	QString scalarBarTitle() const;

public slots:
	void accept() override;

private slots:
	void measuredValueChanged(int index);
	void showColorBarDialog();

private:
	Ui::MeasuredDataPointSettingDialog* ui;

	MeasuredDataPointGroupDataItem::Setting m_setting;
	QMap<QString, QString> m_scalarBarTitleMap;
	QMap<QString, LookupTableContainer*> m_lookupTables;
	LookupTableContainer m_lookupTable;
	QList<QString> m_measuredValues;
};

#endif // MEASUREDDATAPOINTSETTINGDIALOG_H
