#include "colormapsettingeditwidget.h"
#include "ui_colormapsettingeditwidget.h"
#include "private/colormapsettingeditwidget_colortablecontroller.h"
#include "private/colormapsettingeditwidget_switchtodiscretedialog.h"

#include <misc/iricrootpath.h>
#include <misc/informationdialog.h>
#include <misc/lastiodirectory.h>
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
	connect(ui->colorModeAutoRadioButton, &QRadioButton::clicked, this, &ColorMapSettingEditWidget::switchToAutoMode);
	connect(ui->colorModeManualRadioButton, &QRadioButton::clicked, this, &ColorMapSettingEditWidget::switchToManualMode);
	connect<void(QComboBox::*)(int)>(ui->colorPatternSelectComboBox, &QComboBox::currentIndexChanged, this, &ColorMapSettingEditWidget::selectColorMapSetting);
	connect(ui->switchModeButton, &QPushButton::clicked, [=](bool) {this->switchTransitionMode();});

	m_colorTableController = new ColorTableController(this);

	setupColorPatternSelectComboBox();
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
	applySettingOtherThanLegend();
	ui->legendWidget->setSetting(m_concreteSetting.legend);
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
	auto fname = QFileDialog::getOpenFileName(this, tr("Select file to import"),
											  LastIODirectory::get(), tr("Colormap setting (*.cmsetting)"));
	if (fname.isNull()) {return;}

	importSetting(fname, false);
}

void ColorMapSettingEditWidget::exportSetting()
{
	auto fname = QFileDialog::getSaveFileName(this, tr("Input file name to export"),
											  LastIODirectory::get(), tr("Colormap setting (*.cmsetting)"));
	if (fname.isNull()) {return;}

	QFile f(fname);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {
		auto msg = tr("Error occured while opening %1")
				.arg(QDir::toNativeSeparators(fname));
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}

	auto s = concreteSetting();
	QXmlStreamWriter writer(&f);
	writer.writeStartDocument();
	writer.writeStartElement("ColorMapSetting");
	s.save(writer);
	writer.writeEndElement();
	writer.writeEndDocument();
	f.close();

	QFileInfo info(f);
	LastIODirectory::set(info.absolutePath());
}

void ColorMapSettingEditWidget::selectColorMapSetting(int index)
{
	ui->colorPatternSelectComboBox->blockSignals(true);
	ui->colorPatternSelectComboBox->setCurrentIndex(0);
	ui->colorPatternSelectComboBox->blockSignals(false);

	auto fname = m_importTargetFileNames.at(index - 1);
	importSetting(fname, ui->ignoreColorBarSettingCheckBox->isChecked());
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
		colors.insert(colors.begin(), colors.at(0));
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

	for (unsigned int i = 0; i < colors.size(); ++i) {
		const auto& c = colors.at(i);
		if (rowSet.find(i) != rowSet.end()) {
			continue;
		}
		newColors.push_back(c);
	}

	colors = newColors;

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

void ColorMapSettingEditWidget::importSetting(const QString& fileName, bool ignoreLegendSetting)
{
	ColorMapSettingContainer backup = m_concreteSetting;

	QFile f(fileName);
	QDomDocument doc;

	QString errorStr;
	int errorLine;
	int errorColumn;

	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		auto msg = tr("Error occured while loading %1\nParse error %2 at %3, column %4")
				.arg(QDir::toNativeSeparators(fileName))
				.arg(errorStr).arg(errorLine).arg(errorColumn);
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
	m_concreteSetting.load(doc.documentElement());
	if (ignoreLegendSetting) {
		m_concreteSetting.legend = backup.legend;
	}
	m_concreteSetting.valueCaption = backup.valueCaption;

	applySettingOtherThanLegend();
	ui->legendWidget->setSetting(m_concreteSetting.legend);

	QFileInfo info(f);
	LastIODirectory::set(info.absolutePath());
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
	ui->valueRangeGroupBox->setEnabled(s.valueMode == ColorMapSettingContainer::ValueMode::Relative);

	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {
		ui->colorsTableView->setEditTriggers(QTableView::NoEditTriggers);
	} else {
		ui->colorsTableView->setEditTriggers(QTableView::AllEditTriggers);
	}

	ui->fillUpperCheckBox->setChecked(s.fillUpper);
	ui->fillLowerCheckBox->setChecked(s.fillLower);

	m_colorTableController->applyToTable();

	updateSwitchButtonText();
	updateLegendNumberOfLabels();
}

void ColorMapSettingEditWidget::updateLegendNumberOfLabels()
{
	const auto& s = m_concreteSetting;
	/*
	if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		ui->legendWidget->setNumberOfLabels(s.colors.size());
	} else {
		ui->legendWidget->setNumberOfLabels(s.colors.size() + 1);
	}
	*/
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

void ColorMapSettingEditWidget::setupColorPatternSelectComboBox()
{
	QDir iricDir(iRICRootPath::get());
	iricDir.cdUp();
	iricDir.cdUp();

	ui->colorPatternSelectComboBox->blockSignals(true);

	ui->colorPatternSelectComboBox->addItem(tr("(Please select item to apply)"));

	addSettingFileNames(iricDir.absoluteFilePath("colormaps"), ui->colorPatternSelectComboBox, &m_importTargetFileNames);
	addSettingFileNames(iricDir.absoluteFilePath("private/colormaps"), ui->colorPatternSelectComboBox, &m_importTargetFileNames);

	ui->colorPatternSelectComboBox->blockSignals(false);
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
	InformationDialog::warning(this, tr("Warning"), tr("When switching to Gradation Mode, the values are set to the middle value of each sections."), "colormap_to_continuous");
	m_concreteSetting.transitionMode = ColorMapSettingContainer::TransitionMode::Continuous;

	std::vector<ColorMapSettingValueColorPairContainer> newColors;

	for (int i = 0; i < m_concreteSetting.colors.size(); ++i) {
		ColorMapSettingValueColorPairContainer c;
		c.color = m_concreteSetting.colors.at(i).color;
		if (i == 0) {
			c.value = m_concreteSetting.colorTableMinValue;
		} else if (i == m_concreteSetting.colors.size() - 1) {
			c.value = m_concreteSetting.colors.at(i).value;
		} else if (i == 1) {
			double v1 = m_concreteSetting.colorTableMinValue;
			double v2 = m_concreteSetting.colors.at(0).value;
			c.value = (v1 + v2) * 0.5;
		} else {
			double v1 = m_concreteSetting.colors.at(i - 1).value;
			double v2 = m_concreteSetting.colors.at(i).value;
			c.value = (v1 + v2) * 0.5;
		}
		newColors.push_back(c);
	}
	m_concreteSetting.colors = newColors;

	applySettingOtherThanLegend();
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
	m_concreteSetting.colors = dialog.newColors();
	m_concreteSetting.valueMode = ColorMapSettingContainer::ValueMode::Relative;
	m_concreteSetting.transitionMode = ColorMapSettingContainer::TransitionMode::Discrete;

	applySettingOtherThanLegend();
}

void ColorMapSettingEditWidget::setupWidget()
{
	ColorMapSettingContainer newSetting;

	if (m_legendSetting != nullptr) {
		auto ls = dynamic_cast <ColorMapLegendSettingContainer*> (m_legendSetting);
		newSetting = *(ls->colorMapSetting());
		newSetting.legend = *ls;
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapSettingContainer*> (m_setting);
		newSetting = *s;
	}

	setConcreteSetting(newSetting);
}
