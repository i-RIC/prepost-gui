#include "colormapsettingeditwidget_changenumcolorsdialog.h"
#include "colormapsettingeditwidget_changenumcolorsdialog_delegate.h"
#include "ui_colormapsettingeditwidget_changenumcolorsdialog.h"

namespace {

const int COLORWIDTH = 50;

} // namespace

ColorMapSettingEditWidget::ChangeNumColorsDialog::ChangeNumColorsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ColorMapSettingEditWidget_ChangeNumColorsDialog)
{
	ui->setupUi(this);
	connect(ui->minValueEdit, &RealNumberEditWidget::valueChanged, [=](double){updateTable();});
	connect(ui->maxValueEdit, &RealNumberEditWidget::valueChanged, [=](double){updateTable();});
	connect<void(QSpinBox::*)(int)>(ui->colorsSpinBox, &QSpinBox::valueChanged, [=](int){updateTable();});
	connect<void(QSpinBox::*)(int)>(ui->decimalSpinBox, &QSpinBox::valueChanged, [=](int){updateTable();});

	ui->colorsTableView->setModel(&m_model);
	auto delegate = new Delegate(this);
	ui->colorsTableView->setItemDelegate(delegate);
}

ColorMapSettingEditWidget::ChangeNumColorsDialog::~ChangeNumColorsDialog()
{
	delete ui;
}

std::vector<ColorMapSettingValueColorPairContainer> ColorMapSettingEditWidget::ChangeNumColorsDialog::newColors() const
{
	return m_newColors;
}

double ColorMapSettingEditWidget::ChangeNumColorsDialog::minValue() const
{
	return ui->minValueEdit->value();
}

double ColorMapSettingEditWidget::ChangeNumColorsDialog::maxValue() const
{
	return ui->maxValueEdit->value();
}

void ColorMapSettingEditWidget::ChangeNumColorsDialog::setMinMax(double min, double max)
{
	ui->minValueEdit->setValue(min);
	ui->maxValueEdit->setValue(max);

	updateTable();
}

void ColorMapSettingEditWidget::ChangeNumColorsDialog::setOriginalColors(const std::vector<ColorMapSettingValueColorPairContainer>& colors)
{
	double min = colors.at(0).value;
	double max = colors.at(colors.size() - 1).value;
	double range_width = max - min;
	if (range_width == 0) {range_width = 1;}

	m_originalColors = colors;
	for (auto& c : m_originalColors) {
		c.value = (c.value - min) / range_width;
	}
	ui->colorsSpinBox->setValue(static_cast<int> (colors.size()));
}

void ColorMapSettingEditWidget::ChangeNumColorsDialog::updateTable()
{
	double min = ui->minValueEdit->value();
	double max = ui->maxValueEdit->value();
	int colNum = ui->colorsSpinBox->value();
	int decimals = ui->decimalSpinBox->value();

	m_newColors.clear();
	double origStep = 1.0 / (m_originalColors.size() - 1);
	double step = 1.0 / (colNum - 1);

	for (int i = 0; i < colNum; ++i) {
		double v = i * step;
		auto id1 = static_cast<unsigned int> (v / origStep);
		double r = (v - (id1 * origStep)) / origStep;

		ColorMapSettingValueColorPairContainer newC;
		if (id1 == m_originalColors.size() - 1) {
			newC.color = m_originalColors.at(m_originalColors.size() - 1).color;
		} else {
			auto c1 = m_originalColors.at(id1).color.value();
			auto c2 = m_originalColors.at(id1 + 1).color.value();

			double red = c1.redF() * (1 - r) + c2.redF() * r;
			double green = c1.greenF() * (1 - r) + c2.greenF() * r;
			double blue = c1.blueF() * (1 - r) + c2.blueF() * r;

			newC.color = QColor(static_cast<int>(255 * red), static_cast<int>(255 * green), static_cast<int>(255 * blue));
		}
		m_newColors.push_back(newC);
	}

	for (int i = 0; i < m_newColors.size(); ++i) {
		double v = i / static_cast<double> (m_newColors.size() - 1);
		double v2 = QString::number(min + (max - min) * v, 'f', decimals).toDouble();
		double v3 = (v2 - min) / (max - min);
		m_newColors[i].value = v3;
	}
	m_model.setColumnCount(2);
	m_model.setRowCount(static_cast<int> (m_newColors.size()));

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Value"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));

	ui->colorsTableView->setColumnWidth(1, COLORWIDTH);

	for (int i = 0; i < static_cast<int> (m_newColors.size()); ++i) {
		auto itemV = new QStandardItem();
		itemV->setData(min + (max - min) * m_newColors.at(i).value.value(), Qt::EditRole);
		m_model.setItem(static_cast<int> (m_newColors.size()) - 1 - i, 0, itemV);

		auto itemC = new QStandardItem();
		itemC->setData(m_newColors.at(i).color.value(), Qt::EditRole);
		m_model.setItem(static_cast<int> (m_newColors.size()) - 1 - i, 1, itemC);
	}
}
