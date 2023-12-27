#include "colormapsettingeditwidget.h"
#include "ui_colormapsettingeditwidget.h"
#include "private/colormapsettingeditwidget_changenumcolorsdialog.h"
#include "private/colormapsettingeditwidget_colortablecontroller.h"
#include "private/colormapsettingeditwidget_switchtodiscretedialog.h"
#include "private/colormapsettingeditwidget_importdialog.h"

#include <misc/iricrootpath.h>
#include <misc/informationdialog.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuechangert.h>
#include <misc/valuemodifycommandt.h>

#include <QColor>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>

namespace {

void addSettingFileNames(const QString& folderName, QComboBox* combobox, std::vector<QString>* fileNames)
{
	QDir dir(folderName);
	QStringList files = dir.entryList(QStringList(), QDir::Files, QDir::Name);

	for (const QString& file : files) {
		QFileInfo finfo(dir.absoluteFilePath(file));
		if (finfo.suffix() != "cmsetting") {continue;}

		combobox->addItem(finfo.baseName());
		fileNames->push_back(finfo.absoluteFilePath());
	}
}

QString privateColormapsPath()
{
	QDir dir(iRICRootPath::get());
	dir.cdUp();
	dir.cdUp();
	dir.cd("private/colormaps");
	return dir.absolutePath();
}

} // namespace

ColorMapSettingEditWidget::ColorMapSettingEditWidget(QWidget *parent) :
	ColorMapSettingEditWidgetI(parent),
	m_applying {false},
	m_colorTableController {nullptr},
	ui(new Ui::ColorMapSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->autoValueRangeCheckBox, &QCheckBox::toggled, this, &ColorMapSettingEditWidget::switchAutoValueRange);
	connect(ui->fillLowerCheckBox, &QCheckBox::toggled, this, &ColorMapSettingEditWidget::changeFillLower);
	connect(ui->fillUpperCheckBox, &QCheckBox::toggled, this, &ColorMapSettingEditWidget::changeFillUpper);
	connect(ui->minValueEdit, &RealNumberEditWidget::valueChanged, this, &ColorMapSettingEditWidget::changeMinValue);
	connect(ui->maxValueEdit, &RealNumberEditWidget::valueChanged, this, &ColorMapSettingEditWidget::changeMaxValue);
	connect(ui->addButton, &QPushButton::clicked, this, &ColorMapSettingEditWidget::addColor);
	connect(ui->removeButton, &QPushButton::clicked, this, &ColorMapSettingEditWidget::removeColor);
	connect(ui->reverseButton, &QPushButton::clicked, this, &ColorMapSettingEditWidget::reverseColors);
	connect(ui->equalDevisionButton, &QPushButton::clicked, this, &ColorMapSettingEditWidget::divideEqually);
	connect(ui->numColorsButton, &QPushButton::clicked, this, &ColorMapSettingEditWidget::changeNumColors);
	connect(ui->colorModeAutoRadioButton, &QRadioButton::clicked, this, &ColorMapSettingEditWidget::switchToAutoMode);
	connect(ui->colorModeManualRadioButton, &QRadioButton::clicked, this, &ColorMapSettingEditWidget::switchToManualMode);
	connect(ui->switchModeButton, &QPushButton::clicked, [=](bool) {this->switchTransitionMode();});

	ui->minValueEdit->setInformChangeOnFocusOutOnly(true);
	ui->maxValueEdit->setInformChangeOnFocusOutOnly(true);

	m_colorTableController = new ColorTableController(this);
}

ColorMapSettingEditWidget::~ColorMapSettingEditWidget()
{
	delete m_colorTableController;
	delete ui;
}

ColorMapSettingContainer ColorMapSettingEditWidget::concreteSetting() const
{
	ColorMapSettingContainer s;

	s.transitionMode = m_concreteSetting.transitionMode;
	s.autoValueRange = ui->autoValueRangeCheckBox->isChecked();

	if (! s.autoValueRange) {
		s.minValue = ui->minValueEdit->value();
		s.maxValue = ui->maxValueEdit->value();
	}
	s.fillLower = ui->fillLowerCheckBox->isChecked();
	s.fillUpper = ui->fillUpperCheckBox->isChecked();

	s.valueMode = m_concreteSetting.valueMode;
	s.colorTableMinValue = m_concreteSetting.colorTableMinValue;
	s.colors = m_concreteSetting.colors;
	s.autoMinValue = m_concreteSetting.autoMinValue;
	s.autoMaxValue = m_concreteSetting.autoMaxValue;

	s.legend = ui->legendWidget->setting();

	s.valueCaption = m_concreteSetting.valueCaption;

	return s;
}

void ColorMapSettingEditWidget::setConcreteSetting(const ColorMapSettingContainer& setting)
{
	m_concreteSetting = setting;
	m_concreteSetting.legend.copyValue(setting.legend);

	applySettingOtherThanLegend();
	ui->legendWidget->setSetting(m_concreteSetting.legend);
}

void ColorMapSettingEditWidget::setDisableOtherThanLegendVisible(bool disabled)
{
	ui->colorModeGroupBox->setDisabled(disabled);
	//ui->valueRangeGroupBox->setDisabled(disabled);
	ui->autoValueRangeCheckBox->setDisabled(disabled);
	ui->maxValueEdit->setDisabled(disabled);
	ui->minValueEdit->setDisabled(disabled);
	ui->colorsGroupBox->setDisabled(disabled);
	ui->switchModeButton->setDisabled(disabled);

	ui->legendWidget->setDisableOtherThanVisible(disabled);
}

QUndoCommand* ColorMapSettingEditWidget::createModifyCommand() const
{
	if (m_legendSetting != nullptr) {
		auto command = new MergeSupportedListCommand(iRIC::generateCommandId("ColorMapSettingEditWidget::createModifyCommand_Multi"), true);
		auto newSetting = concreteSetting();

		auto ls = dynamic_cast<ColorMapLegendSettingContainer*> (m_legendSetting);
		command->addCommand(new ValueModifyCommmand<ColorMapLegendSettingContainer> (
					iRIC::generateCommandId("Edit Legend Setting"), true,
					newSetting.legend, ls));

		auto s = ls->colorMapSetting();
		newSetting.legend = s->legend;

		command->addCommand(new ValueModifyCommmand<ColorMapSettingContainer> (
					iRIC::generateCommandId("Edit Colormap Setting"), true,
					newSetting, s));

		return command;
	} else if (m_setting != nullptr) {
		return new ValueModifyCommmand<ColorMapSettingContainer>(
					iRIC::generateCommandId("ColorMapSettingEditWidget::createModifyCommand_Single"), true,
					concreteSetting(), dynamic_cast<ColorMapSettingContainer*> (m_setting)
		);
	}
	return nullptr;
}

void ColorMapSettingEditWidget::importSetting()
{
	ImportDialog dialog(this);
	dialog.setOriginalSetting(concreteSetting());

	auto ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_concreteSetting = dialog.setting();

	applySettingOtherThanLegend();
	ui->legendWidget->setSetting(m_concreteSetting.legend);
}

void ColorMapSettingEditWidget::exportSetting()
{
	auto fname = QFileDialog::getSaveFileName(this, tr("Input file name to export"),
												privateColormapsPath(), tr("Colormap setting (*.cmsetting)"));
	if (fname.isNull()) {return;}

	bool ok = m_concreteSetting.exportData(fname);
	if (! ok) {
		auto msg = tr("Error occured while opening %1")
				.arg(QDir::toNativeSeparators(fname));
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
}

void ColorMapSettingEditWidget::switchToAutoMode()
{
	if (m_applying) {return;}

	auto& s = m_concreteSetting;
	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {return;}

	s.valueMode = ColorMapSettingContainer::ValueMode::Relative;

	s.autoValueRange = false;

	double min = s.colorTableMinValue;
	if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		min = s.colors[0].value;
	}
	double max = s.colors[s.colors.size() - 1].value;

	s.minValue = min;
	s.maxValue = max;

	s.colorTableMinValue = 0;
	auto newColors = s.colors;
	double range_width = (max - min);
	if (range_width == 0) {range_width = 1;} // to avoid nan

	for (int i = 0; i < newColors.size(); ++i) {
		newColors[i].value = (s.colors[i].value - min) / range_width;
	}
	s.colors = newColors;

	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::switchToManualMode()
{
	if (m_applying) {return;}

	auto& s = m_concreteSetting;
	if (s.valueMode == ColorMapSettingContainer::ValueMode::Absolute) {return;}

	s.valueMode = ColorMapSettingContainer::ValueMode::Absolute;

	double min = s.minValue;
	double max = s.maxValue;
	if (s.autoValueRange) {
		min = s.autoMinValue;
		max = s.autoMaxValue;
	}

	s.colorTableMinValue = min;
	auto newColors = s.colors;
	for (int i = 0; i < newColors.size(); ++i) {
		newColors[i].value = min + (max - min) * s.colors[i].value;
	}
	s.colors = newColors;

	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::switchTransitionMode()
{
	if (m_concreteSetting.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		switchTransitionModeToDiscrete();
	} else {
		switchTransitionModeToContinuous();
	}
	ui->legendWidget->updateAutoNumberOfLabels();
}

void ColorMapSettingEditWidget::reverseColors()
{
	auto tmpColors = m_concreteSetting.colors;
	for (int i = 0; i < tmpColors.size(); ++i) {
		m_concreteSetting.colors[tmpColors.size() - 1 - i].color = tmpColors[i].color;
	}
	m_colorTableController->applyToTable();
}

void ColorMapSettingEditWidget::divideEqually()
{
	auto& colors = m_concreteSetting.colors;
	if (m_concreteSetting.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		double min = colors.at(0).value.value();
		double max = colors.at(colors.size() - 1).value.value();

		for (int i = 1; i < colors.size() - 1; ++i) {
			colors[i].value = min + (max - min) * i / (colors.size() - 1);
		}
	} else {
		double min = m_concreteSetting.colorTableMinValue;
		double max = colors.at(colors.size() - 1).value.value();

		for (int i = 0; i < colors.size() - 1; ++i) {
			colors[i].value = min + (max - min) * (i + 1) / colors.size();
		}
	}

	m_colorTableController->applyToTable();
}

void ColorMapSettingEditWidget::changeNumColors()
{
	ChangeNumColorsDialog dialog(this);
	dialog.setOriginalColors(m_concreteSetting.colors);
	dialog.setMinMax(m_concreteSetting.getMinValue(), m_concreteSetting.getMaxValue());
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_concreteSetting.autoValueRange = false;
	m_concreteSetting.minValue = dialog.minValue();
	m_concreteSetting.maxValue = dialog.maxValue();
	m_concreteSetting.colorTableMinValue = 0;
	m_concreteSetting.colors = dialog.newColors();
	m_concreteSetting.valueMode = ColorMapSettingContainer::ValueMode::Relative;
	m_concreteSetting.transitionMode = ColorMapSettingContainer::TransitionMode::Continuous;

	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::switchAutoValueRange(bool automatic)
{
	if (m_applying) {return;}

	m_concreteSetting.autoValueRange = automatic;
	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::changeFillLower(bool fill)
{
	if (m_applying) {return;}

	m_concreteSetting.fillLower = fill;
}

void ColorMapSettingEditWidget::changeFillUpper(bool fill)
{
	if (m_applying) {return;}

	m_concreteSetting.fillUpper = fill;
}

void ColorMapSettingEditWidget::changeMinValue(double min)
{
	if (m_applying) {return;}

	m_concreteSetting.minValue = min;
	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::changeMaxValue(double max)
{
	if (m_applying) {return;}

	m_concreteSetting.maxValue = max;
	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::addColor()
{
	int row = ui->colorsTableView->currentIndex().row();
	if (row == -1) {return;}

	auto& colors = m_concreteSetting.colors;
	row = colors.size() - 1 - row;

	if (row == 0) {
		if (m_concreteSetting.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
			auto newColor = colors.at(0);
			auto delta = colors.at(1).value - colors.at(0).value;
			newColor.value -= delta;

			colors.insert(colors.begin(), newColor);
		} else {
			auto newColor = colors.at(0);
			auto delta = colors.at(0).value - m_concreteSetting.colorTableMinValue;
			newColor.value = m_concreteSetting.colorTableMinValue;

			colors.insert(colors.begin(), newColor);
			m_concreteSetting.colorTableMinValue -= delta;
		}
		normalizeColors();
	} else {
		ColorMapSettingValueColorPairContainer newColor;
		newColor.value = (colors.at(row - 1).value + colors.at(row).value) * 0.5;
		int r = (colors.at(row - 1).color.value().red() + colors.at(row).color.value().red()) / 2;
		int g = (colors.at(row - 1).color.value().green() + colors.at(row).color.value().green()) / 2;
		int b = (colors.at(row - 1).color.value().blue() + colors.at(row).color.value().blue()) / 2;
		newColor.color = QColor(r, g, b);
		colors.insert(colors.begin() + row, newColor);
	}

	m_colorTableController->applyToTable();

	updateLegendNumberOfLabels();
}

void ColorMapSettingEditWidget::removeColor()
{
	auto& colors = m_concreteSetting.colors;
	std::vector<ColorMapSettingValueColorPairContainer> newColors;

	auto rows = ui->colorsTableView->selectionModel()->selectedRows();
	std::unordered_set<unsigned int> rowSet;
	for (auto r : rows) {
		rowSet.insert(colors.size() - 1 - r.row());
	}

	int firstCol = -1;
	for (unsigned int i = 0; i < colors.size(); ++i) {
		const auto& c = colors.at(i);
		if (rowSet.find(i) != rowSet.end()) {
			continue;
		}
		newColors.push_back(c);
		if (firstCol == -1) {firstCol = i;}
	}
	if (firstCol != 0) {
		m_concreteSetting.colorTableMinValue = colors.at(firstCol - 1).value;
	}

	colors = newColors;
	normalizeColors();

	m_colorTableController->applyToTable();

	updateLegendNumberOfLabels();
}

QTableView* ColorMapSettingEditWidget::colorTable() const
{
	return ui->colorsTableView;
}

QPushButton* ColorMapSettingEditWidget::removeButton() const
{
	return ui->removeButton;
}

void ColorMapSettingEditWidget::applySettingOtherThanLegend()
{
	ValueChangerT<bool> applyChanger(&m_applying, true);

	fixData();

	auto& s = m_concreteSetting;

	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {
		ui->colorModeAutoRadioButton->setChecked(true);
	} else if (s.valueMode == ColorMapSettingContainer::ValueMode::Absolute) {
		ui->colorModeManualRadioButton->setChecked(true);
	}

	ui->autoValueRangeCheckBox->setChecked(s.autoValueRange);
	if (s.autoValueRange) {
		ui->minValueEdit->setValue(s.autoMinValue);
		ui->maxValueEdit->setValue(s.autoMaxValue);
		s.minValue = s.autoMinValue;
		s.maxValue = s.autoMaxValue;
	} else {
		ui->minValueEdit->setValue(s.minValue);
		ui->maxValueEdit->setValue(s.maxValue);
	}
	ui->minValueEdit->setMaximum(ui->maxValueEdit->value());
	ui->maxValueEdit->setMinimum(ui->minValueEdit->value());

	ui->valueRangeGroupBox->setEnabled(s.valueMode == ColorMapSettingContainer::ValueMode::Relative);

	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {
		ui->colorsTableView->setEditTriggers(QTableView::NoEditTriggers);
	} else {
		ui->colorsTableView->setEditTriggers(QTableView::AllEditTriggers);
	}

	ui->fillUpperCheckBox->setChecked(s.fillUpper);
	ui->fillLowerCheckBox->setChecked(s.fillLower);

	m_colorTableController->applyToTable();

	ui->numColorsButton->setEnabled(s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous);

	updateSwitchButtonText();
	updateLegendNumberOfLabels();
}

void ColorMapSettingEditWidget::updateLegendNumberOfLabels()
{
	ui->legendWidget->updateNumberOfLabelsIfNeeded();
}

void ColorMapSettingEditWidget::fixData()
{
	auto& s = m_concreteSetting;

	// if the value mode is relative, min value should be 0, max value should be 1.

	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {
		s.colorTableMinValue = 0;

		if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
			s.colors.at(0).value = 0;
		}
		s.colors.at(s.colors.size() - 1).value = 1;
	}
}

void ColorMapSettingEditWidget::updateSwitchButtonText()
{
	QString text;
	if (m_concreteSetting.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		text = tr("Switch to Discrete Mode");
	} else {
		text = tr("Switch to Gradation Mode");
	}
	ui->switchModeButton->setText(text);
}

void ColorMapSettingEditWidget::switchTransitionModeToContinuous()
{
	m_concreteSetting.transitionMode = ColorMapSettingContainer::TransitionMode::Continuous;

	std::vector<ColorMapSettingValueColorPairContainer> newColors;

	const auto& colors = m_concreteSetting.colors;
	double min = m_concreteSetting.colorTableMinValue;
	double max = colors.at(colors.size() - 1).value;
	double step = (max - min) / (colors.size() - 1);
	for (unsigned int i = 0; i < colors.size(); ++i) {
		ColorMapSettingValueColorPairContainer c;
		c.color = colors.at(i).color;
		if (i == colors.size() - 1) {
			c.value = max;
		} else {
			c.value = min + i * step;
		}
		newColors.push_back(c);
	}
	m_concreteSetting.colors = newColors;

	applySettingOtherThanLegend();
	ui->legendWidget->updateLabelNumberWidgetVisibility();
}

void ColorMapSettingEditWidget::switchTransitionModeToDiscrete()
{
	SwitchToDiscreteDialog dialog(this);
	dialog.setOriginalColors(m_concreteSetting.colors);
	dialog.setMinMax(m_concreteSetting.getMinValue(), m_concreteSetting.getMaxValue());
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_concreteSetting.autoValueRange = false;
	m_concreteSetting.minValue = dialog.minValue();
	m_concreteSetting.maxValue = dialog.maxValue();
	m_concreteSetting.colorTableMinValue = 0;
	m_concreteSetting.colors = dialog.newColors();
	m_concreteSetting.valueMode = ColorMapSettingContainer::ValueMode::Relative;
	m_concreteSetting.transitionMode = ColorMapSettingContainer::TransitionMode::Discrete;

	applySettingOtherThanLegend();
	ui->legendWidget->updateLabelNumberWidgetVisibility();
}

void ColorMapSettingEditWidget::normalizeColors()
{
	if (m_concreteSetting.valueMode == ColorMapSettingContainer::ValueMode::Absolute) {return;}
	auto& colors = m_concreteSetting.colors;
	double min, max;
	if (m_concreteSetting.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (colors.at(0).value.value() == 0 && colors.at(colors.size() - 1).value.value() == 1) {return;}
		min = colors.at(0).value.value();
		max = colors.at(colors.size() - 1).value.value();
	} else {
		if (m_concreteSetting.colorTableMinValue.value() == 0 && colors.at(colors.size() - 1).value.value() == 1) {return;}
		min = m_concreteSetting.colorTableMinValue;
		max = colors.at(colors.size() - 1).value.value();
	}
	if (! m_concreteSetting.autoValueRange) {
		double oldMin = m_concreteSetting.minValue;
		double oldMax = m_concreteSetting.maxValue;
		double newMin = oldMin + (oldMax - oldMin) * min;
		double newMax = oldMin + (oldMax - oldMin) * max;

		m_concreteSetting.minValue = newMin;
		m_concreteSetting.maxValue = newMax;

		ui->minValueEdit->setValue(newMin);
		ui->maxValueEdit->setValue(newMax);
	}
	auto offset = - min;
	auto scale = 1 / (max - min);

	m_concreteSetting.colorTableMinValue = 0;
	for (auto& c : colors) {
		c.value = (c.value + offset) * scale;
	}
}

void ColorMapSettingEditWidget::setupWidget()
{
	ColorMapSettingContainer newSetting;

	if (m_legendSetting != nullptr) {
		auto ls = dynamic_cast <ColorMapLegendSettingContainer*> (m_legendSetting);
		newSetting = *(ls->colorMapSetting());
		newSetting.legend.copyWithColorMap(*ls);
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapSettingContainer*> (m_setting);
		newSetting = *s;
	}

	setConcreteSetting(newSetting);
}

void ColorMapSettingEditWidget::updateImageSetting()
{
	ColorMapLegendSettingContainer newSetting;

	if (m_legendSetting != nullptr) {
		auto ls = dynamic_cast <ColorMapLegendSettingContainer*> (m_legendSetting);
		if (ls->delegateMode()) {
			newSetting = ls->colorMapSetting()->legend;
		} else {
			newSetting = *ls;
		}
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapSettingContainer*> (m_setting);
		newSetting = s->legend;
	}

	ui->legendWidget->setImageSetting(newSetting.imageSetting);
}
