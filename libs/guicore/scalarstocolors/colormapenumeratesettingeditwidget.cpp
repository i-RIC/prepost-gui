#include "colormapenumeratesettingeditwidget.h"
#include "ui_colormapenumeratesettingeditwidget.h"
#include "private/colormapenumeratesettingeditwidget_colortablecontroller.h"
#include "../solverdef/enumloader.h"

#include <misc/lastiodirectory.h>
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
	applySetting();
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
	ColorMapEnumerateSettingContainer backup = m_concreteSetting;

	auto fname = QFileDialog::getOpenFileName(this, tr("Select file to import"),
																						LastIODirectory::get(), tr("Colormap setting (*.cmsetting)"));
	if (fname.isNull()) {return;}

	QFile f(fname);
	QDomDocument doc;

	QString errorStr;
	int errorLine;
	int errorColumn;

	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		auto msg = tr("Error occured while loading %1\nParse error %2 at %3, column %4")
				.arg(QDir::toNativeSeparators(fname))
				.arg(errorStr).arg(errorLine).arg(errorColumn);
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
	m_concreteSetting.load(doc.documentElement());

	applySetting();

	QFileInfo info(f);
	LastIODirectory::set(info.absolutePath());
}

void ColorMapEnumerateSettingEditWidget::exportSetting()
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
	s.save(writer);
	f.close();

	QFileInfo info(f);
	LastIODirectory::set(info.absolutePath());
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
		newSetting.legend = *ls;
	} else if (m_setting != nullptr) {
		auto s = dynamic_cast <ColorMapEnumerateSettingContainer*> (m_setting);
		newSetting = *s;
	}

	setConcreteSetting(newSetting);
}
