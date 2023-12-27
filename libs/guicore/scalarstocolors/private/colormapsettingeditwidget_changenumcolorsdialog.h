#ifndef COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_H
#define COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_H

#include "../colormapsettingeditwidget.h"

#include <QDialog>

namespace Ui {
class ColorMapSettingEditWidget_ChangeNumColorsDialog;
}

class ColorMapSettingEditWidget::ChangeNumColorsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ChangeNumColorsDialog(QWidget *parent = nullptr);
	~ChangeNumColorsDialog();

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

	Ui::ColorMapSettingEditWidget_ChangeNumColorsDialog *ui;

	class Delegate;
};

#endif // COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_H
