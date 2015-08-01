#ifndef MEASUREDDATAPOINTSETTINGDIALOG_H
#define MEASUREDDATAPOINTSETTINGDIALOG_H

#include <guibase/contoursettingwidget.h>
#include "../scalarstocolors/lookuptablecontainer.h"
#include "measureddatapointgroupdataitem.h"
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
//	void setScalarBarSetting(const ScalarBarSetting& setting);
//	void setCurrentMeasuredValue(QString sol);
//	void setContour(ContourSettingWidget::Contour c);
//	void setNumberOfDivision(int div);
//	void setPointSize(int pointSize);
//	void setFillUpper(bool fill);
//	void setFillLower(bool fill);
//	void setOpacityPercent(int o);
	MeasuredDataPointGroupDataItem::Setting setting() const;
	LookupTableContainer lookupTable() const;
	QString scalarBarTitle() const;
//	QString currentMeasuredValue();
//	ContourSettingWidget::Contour contour();
//	int numberOfDivision();
//	const LookupTableContainer& lookupTable();
//	int pointSize();
//	bool fillUpper();
//	bool fillLower();
//	int opacityPercent();

//	const ScalarBarSetting scalarBarSetting();
//	QString scalarBarTitle();

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
//	vtkTextPropertySettingContainer m_titleTextSetting;
//	vtkTextPropertySettingContainer m_labelTextSetting;
	LookupTableContainer m_lookupTable;
	QList<QString> m_measuredValues;
};

#endif // MEASUREDDATAPOINTSETTINGDIALOG_H
