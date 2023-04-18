#include "colormapenumeratesettingeditwidget.h"
#include "ui_colormapenumeratesettingeditwidget.h"
#include "private/colormapenumeratesettingeditwidget_colortablecontroller.h"
#include "private/colormapenumeratesettingeditwidget_importdialog.h"
#include "../solverdef/enumloader.h"

#include <misc/iricrootpath.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>

namespace {

QString privateColormapsPath()
{
	QDir dir(iRICRootPath::get());
	dir.cdUp();
	dir.cdUp();
	dir.cd("private/colormaps");
	return dir.absolutePath();
}

} // namespace

ColorMapEnumerateSettingEditWidget::ColorMapEnumerateSettingEditWidget(QWidget *parent) :
	ColorMapSettingEditWidgetI(parent),
	m_colorTableController {nullptr},
	ui(new Ui::ColorMapEnumerateSettingEditWidget)
{
	ui->setupUi(this);

	m_colorTableController = new ColorTableController(this);
}

ColorMapEnumerateSettingEditWidget::~ColorMapEnumerateSettingEditWidget()
{
	delete ui;
}

ColorMapEnumerateSettingContainer ColorMapEnumerateSettingEditWidget::concreteSetting() const
{
	ColorMapEnumerateSettingContainer s;

	s.valueCaption = m_concreteSetting.valueCaption;
	s.valueCaptions = m_concreteSetting.valueCaptions;
	s.colors = m_concreteSetting.colors;

	s.legend = ui->legendWidget->setting();

	return s;
}

void ColorMapEnumerateSettingEditWidget::setConcreteSetting(const ColorMapEnumerateSettingContainer& setting)
{
	m_concreteSetting = setting;
	m_concreteSetting.legend.copyWithColorMap(setting.legend);

	applySetting();
}

void ColorMapEnumerateSettingEditWidget::setDisableOtherThanLegendVisible(bool disabled)
{
	ui->colorsGroupBox->setDisabled(disabled);
	ui->legendWidget->setDisableOtherThanImageVisible(disabled);
}

QUndoCommand* ColorMapEnumerateSettingEditWidget::createModifyCommand() const
{
	if (m_legendSetting != nullptr) {
		auto command = new MergeSupportedListCommand(iRIC::generateCommandId("ColorMapEnumerateSettingEditWidget::createModifyCommand_Multi"), true);
		auto newSetting = concreteSetting();

		auto ls = dynamic_cast<ColorMapEnumerateLegendSettingContainer*> (m_legendSetting);
		new ValueModifyCommmand<ColorMapEnumerateLegendSettingContainer> (
					iRIC::generateCommandId("Edit Legend Setting"), true,
					newSetting.legend, ls);

		auto s = ls->colorMapSetting();
		newSetting.legend = s->legend;
		new ValueModifyCommmand<ColorMapEnumerateSettingContainer> (
					iRIC::generateCommandId("Edit Colormap Setting"), true,
					newSetting, s);

		return command;

	} else if (m_setting != nullptr) {
		return new ValueModifyCommmand<ColorMapEnumerateSettingContainer>(
					iRIC::generateCommandId("ColorMapEnumerateSettingEditWidget::createModifyCommand_Single"), true,
					concreteSetting(), dynamic_cast<ColorMapEnumerateSettingContainer*> (m_setting)
		);
	}
	return nullptr;
}

void ColorMapEnumerateSettingEditWidget::importSetting()
{
	ImportDialog dialog(this);
	dialog.setOriginalSetting(m_concreteSetting);

	auto ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_concreteSetting = dialog.setting();

	applySetting();
}

void ColorMapEnumerateSettingEditWidget::exportSetting()
{
	auto fname = QFileDialog::getSaveFileName(this, tr("Input file name to export"),
																						privateColormapsPath(), tr("Enumerate colormap setting (*.ecmsetting)"));
	if (fname.isNull()) {return;}

	bool ok = m_concreteSetting.exportData(fname);
	if (! ok) {
		auto msg = tr("Error occured while opening %1")
				.arg(QDir::toNativeSeparators(fname));
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
}


QTableView* ColorMapEnumerateSettingEditWidget::colorTable() const
{
	return ui->colorsTableView;
}

void ColorMapEnumerateSettingEditWidget::applySetting()
{
	auto& s = m_concreteSetting;

	m_colorTableController->applyToTable();

	ui->legendWidget->setSetting(s.legend);
}

void ColorMapEnumerateSettingEditWidget::setupWidget()
{
	ColorMapEnumerateSettingContainer newSetting;

	if (m_legendSetting != nullptr) {
		auto ls = dynamic_cast <ColorMapEnumerateLegendSettingContainer*> (m_legendSetting);
		newSetting = *(ls->colorMapSetting());
		newSetting.legend.copyWithColorMap(*ls);
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapEnumerateSettingContainer*> (m_setting);
		newSetting = *s;
	}

	setConcreteSetting(newSetting);
}

void ColorMapEnumerateSettingEditWidget::updateImageSetting()
{
	ColorMapEnumerateLegendSettingContainer newSetting;

	if (m_legendSetting != nullptr) {
		auto ls = dynamic_cast <ColorMapEnumerateLegendSettingContainer*> (m_legendSetting);
		if (ls->delegateMode()) {
			newSetting = ls->colorMapSetting()->legend;
		} else {
			newSetting = *ls;
		}
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapEnumerateSettingContainer*> (m_setting);
		newSetting = s->legend;
	}

	ui->legendWidget->setImageSetting(newSetting.imageSetting);
}
