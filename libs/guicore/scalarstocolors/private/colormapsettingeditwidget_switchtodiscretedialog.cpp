#include "colormapsettingeditwidget_switchtodiscretedialog.h"
#include "colormapsettingeditwidget_switchtodiscretedialog_delegate.h"
#include "ui_colormapsettingeditwidget_switchtodiscretedialog.h"

#include <QTableView>

namespace {

const int COLORWIDTH = 50;

} // namespace

ColorMapSettingEditWidget::SwitchToDiscreteDialog::SwitchToDiscreteDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ColorMapSettingEditWidget_SwitchToDiscreteDialog)
{
	ui->setupUi(this);
	connect(ui->minValueEdit, &RealNumberEditWidget::valueChanged, [=](double){updateTable();});
	connect(ui->maxValueEdit, &RealNumberEditWidget::valueChanged, [=](double){updateTable();});
	connect<void(QSpinBox::*)(int)>(ui->colorsSpinBox, &QSpinBox::valueChanged, [=](int){updateTable();});

	ui->colorsTableView->setModel(&m_model);
	auto delegate = new Delegate(this);
	ui->colorsTableView->setItemDelegate(delegate);
}

ColorMapSettingEditWidget::SwitchToDiscreteDialog::~SwitchToDiscreteDialog()
{
	delete ui;
}

std::vector<ColorMapSettingValueColorPairContainer> ColorMapSettingEditWidget::SwitchToDiscreteDialog::newColors() const
{
	return m_newColors;
}

double ColorMapSettingEditWidget::SwitchToDiscreteDialog::minValue() const
{
	return ui->minValueEdit->value();
}

double ColorMapSettingEditWidget::SwitchToDiscreteDialog::maxValue() const
{
	return ui->maxValueEdit->value();
}

void ColorMapSettingEditWidget::SwitchToDiscreteDialog::setMinMax(double min, double max)
{
	ui->minValueEdit->setValue(min);
	ui->maxValueEdit->setValue(max);

	updateTable();
}

void ColorMapSettingEditWidget::SwitchToDiscreteDialog::setOriginalColors(const std::vector<ColorMapSettingValueColorPairContainer>& colors)
{
	double min = colors.at(0).value;
	double max = colors.at(colors.size() - 1).value;
	double range_width = max - min;
	if (range_width == 0) {range_width = 1;}

	m_originalColors = colors;
	for (auto& c : m_originalColors) {
		c.value = (c.value - min) / range_width;
	}
}

void ColorMapSettingEditWidget::SwitchToDiscreteDialog::updateTable()
{
	double min = ui->minValueEdit->value();
	double max = ui->maxValueEdit->value();
	int colNum = ui->colorsSpinBox->value();

	m_newColors.clear();
	for (int i = 0; i < m_originalColors.size() - 1; ++i) {
		m_newColors.push_back(m_originalColors.at(i));

		auto c1 = m_originalColors.at(i).color.value();
		auto c2 = m_originalColors.at(i + 1).color.value();
		for (int j = 0; j < colNum; ++j) {
			double r = (j + 1) / static_cast<double>(colNum + 1);
			double red = c1.redF() * (1 - r) + c2.redF() * r;
			double green = c1.greenF() * (1 - r) + c2.greenF() * r;
			double blue = c1.blueF() * (1 - r) + c2.blueF() * r;

			ColorMapSettingValueColorPairContainer newC;
			newC.color = QColor(static_cast<int>(255 * red), static_cast<int>(255 * green), static_cast<int>(255 * blue));
			m_newColors.push_back(newC);
		}
	}
	m_newColors.push_back(m_originalColors.at(m_originalColors.size() - 1));

	for (int i = 0; i < m_newColors.size(); ++i) {
		m_newColors[i].value = (i + 1) / static_cast<double> (m_newColors.size());
	}
	m_model.setColumnCount(3);
	m_model.setRowCount(m_newColors.size());

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Min value"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Max value"));
	m_model.setHeaderData(2, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));

	ui->colorsTableView->setColumnWidth(2, COLORWIDTH);

	for (int i = 0; i < m_newColors.size(); ++i) {
		auto itemMin = new QStandardItem();
		if (i == 0) {
			itemMin->setData(min, Qt::EditRole);
		} else {
			itemMin->setData(min + (max - min) * m_newColors.at(i - 1).value.value(), Qt::EditRole);
		}
		m_model.setItem(m_newColors.size() - 1 - i, 0, itemMin);

		auto itemMax = new QStandardItem();
		itemMax->setData(min + (max - min) * m_newColors.at(i).value.value(), Qt::EditRole);
		m_model.setItem(m_newColors.size() - 1 - i, 1, itemMax);

		auto itemC = new QStandardItem();
		itemC->setData(m_newColors.at(i).color.value(), Qt::EditRole);
		m_model.setItem(m_newColors.size() - 1 - i, 2, itemC);
	}
}
