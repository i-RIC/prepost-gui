#ifndef COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_H
#define COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_H

#include "../colormapsettingeditwidget.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class ColorMapSettingEditWidget_SwitchToDiscreteDialog;
}

class ColorMapSettingEditWidget::SwitchToDiscreteDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SwitchToDiscreteDialog(QWidget *parent);
	~SwitchToDiscreteDialog();

	std::vector<ColorMapSettingValueColorPairContainer> newColors() const;
	double minValue() const;
	double maxValue() const;

	void setMinMax(double min, double max);
	void setOriginalColors(const std::vector<ColorMapSettingValueColorPairContainer>& colors);

private slots:
	void updateTable();

private:
	std::vector<ColorMapSettingValueColorPairContainer> m_newColors;
	std::vector<ColorMapSettingValueColorPairContainer> m_originalColors;

	QStandardItemModel m_model;

	Ui::ColorMapSettingEditWidget_SwitchToDiscreteDialog *ui;

	class Delegate;
};

#endif // COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_H
