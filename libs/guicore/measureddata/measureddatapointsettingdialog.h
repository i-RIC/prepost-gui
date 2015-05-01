#ifndef MEASUREDDATAPOINTSETTINGDIALOG_H
#define MEASUREDDATAPOINTSETTINGDIALOG_H

#include <guibase/contoursettingwidget.h>
#include "../scalarstocolors/lookuptablecontainer.h"
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
	void setCurrentMeasuredValue(QString sol);
	void setContour(ContourSettingWidget::Contour c);
	void setNumberOfDivision(int div);
	void setLookupTables(const QMap<QString, LookupTableContainer*>& lookuptables) {
		m_lookupTables = lookuptables;
	}
	void setPointSize(int pointSize);
	void setFillUpper(bool fill);
	void setFillLower(bool fill);
	void setOpacityPercent(int o);
	void setNoPolyData(bool noPolyData);

	QString currentMeasuredValue();
	ContourSettingWidget::Contour contour();
	int numberOfDivision();
	const LookupTableContainer& lookupTable();
	int pointSize();
	bool fillUpper();
	bool fillLower();
	int opacityPercent();

	void setScalarBarTitleMap(const QMap<QString, QString>& titlemap);
	void setScalarBarSetting(const ScalarBarSetting& setting);
	const ScalarBarSetting scalarBarSetting();
	QString scalarBarTitle();

public slots:
	void accept();

private slots:
	void measuredValueChanged(int index);
	void showColorBarDialog();

private:
	Ui::MeasuredDataPointSettingDialog* ui;

	QMap<QString, QString> m_scalarBarTitleMap;
	ScalarBarSetting m_scalarBarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
	LookupTableContainer m_lookupTable;
	QMap<QString, LookupTableContainer*> m_lookupTables;
	QList<QString> m_measuredValues;
};

#endif // MEASUREDDATAPOINTSETTINGDIALOG_H
