#ifndef MEASUREDDATAVECTORSETTINGDIALOG_H
#define MEASUREDDATAVECTORSETTINGDIALOG_H

#include <QDialog>

#include "../project/measureddata.h"

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
	bool isEnabled() const;
	void setSolution(const QString& sol);
	QString solution() const;
	void setScalarValue(const QString& scalar);
	QString scalarValue() const;
	void setColor(const QColor& color);
	QColor color() const;

	void setLengthMode(MeasuredData::ArrowLengthMode lm);
	MeasuredData::ArrowLengthMode lengthMode() const;
	void setStandardValue(double stdVal);
	double standardValue() const;
	void setLegendLength(int len);
	int legendLength() const;
	void setMinimumValue(double minVal);
	double minimumValue() const;

	void setColorMode(MeasuredData::ArrowColorMode cm);
	MeasuredData::ArrowColorMode colorMode() const;

private:
	void setupSolutionComboBox(MeasuredData* data);
	QList<QString> m_solutions;
	QList<QString> m_scalars;
	Ui::MeasuredDataVectorSettingDialog* ui;
};

#endif // MEASUREDDATAVECTORSETTINGDIALOG_H
