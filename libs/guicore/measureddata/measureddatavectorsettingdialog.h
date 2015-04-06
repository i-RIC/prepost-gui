#ifndef MEASUREDDATAVECTORSETTINGDIALOG_H
#define MEASUREDDATAVECTORSETTINGDIALOG_H

#include <QDialog>

#include "../project/measureddata.h"

namespace Ui {
	class MeasuredDataVectorSettingDialog;
}

class MeasuredDataVectorSettingDialog : public QDialog {
	Q_OBJECT

public:
	explicit MeasuredDataVectorSettingDialog(QWidget *parent = 0);
	~MeasuredDataVectorSettingDialog();
	void setData(MeasuredData *data);
	bool isEnabled() const;
	void setSolution(const QString& sol);
	const QString solution();
	void setScalarValue(const QString& scalar);
	const QString scalarValue();
	void setColor(const QColor &color);
	const QColor color() const;

	void setLengthMode(MeasuredData::ArrowLengthMode lm);
	MeasuredData::ArrowLengthMode lengthMode();
	void setStandardValue(double stdVal);
	double standardValue();
	void setLegendLength(int len);
	int legendLength();
	void setMinimumValue(double minVal);
	double minimumValue();

	void setColorMode(MeasuredData::ArrowColorMode cm);
	MeasuredData::ArrowColorMode colorMode();

private:
	void setupSolutionComboBox(MeasuredData* data);
	QList<QString> m_solutions;
	QList<QString> m_scalars;
	Ui::MeasuredDataVectorSettingDialog *ui;
};

#endif // MEASUREDDATAVECTORSETTINGDIALOG_H
