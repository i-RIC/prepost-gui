#include "colormapenumeratesettingeditwidget_importdialog.h"
#include "colormapenumeratesettingeditwidget_importdialog_setting.h"
#include "ui_colormapenumeratesettingeditwidget_importdialog.h"

#include <misc/iricrootpath.h>
#include <misc/valuechangert.h>

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QStandardItem>

namespace {

int PathRole = Qt::UserRole + 1;

bool loadModelRecursive(const QString path, QStandardItem* parent)
{
	bool childExists = false;
	QDir dir(path);
	QStringList entries = dir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	for (const auto& entry : entries) {
		QFileInfo finfo(dir.absoluteFilePath(entry));
		auto folder = new QStandardItem(QIcon(":/libs/guibase/images/iconFolder.svg"), finfo.fileName());
		parent->appendRow(folder);
		bool childExists2 = loadModelRecursive(finfo.absoluteFilePath(), folder);
		if (! childExists2) {
			parent->removeRow(folder->row());
		} else {
			childExists = true;
		}
	}

	entries = dir.entryList(QStringList(), QDir::Files, QDir::Name);
	for (const auto& entry : entries) {
		QFileInfo finfo(dir.absoluteFilePath(entry));
		if (finfo.suffix() != "ecmsetting") {continue;}

		auto item = new QStandardItem(QIcon(":/libs/guibase/images/iconPaper.svg"), finfo.fileName());
		item->setData(finfo.absoluteFilePath(), PathRole);
		parent->appendRow(item);
		childExists = true;
	}

	return childExists;
}

} // namespace

ColorMapEnumerateSettingEditWidget::ImportDialog::ImportDialog(QWidget *parent) :
	QDialog(parent),
	m_selectionModel(&m_standardsModel),
	ui(new Ui::ColorMapEnumerateSettingEditWidget_ImportDialog)
{
	ui->setupUi(this);
	ui->fileGroupBox->hide();

	connect(ui->standardRadioButton, &QRadioButton::toggled, ui->standardsGroupBox, &QGroupBox::setVisible);
	connect(ui->fileRadioButton, &QRadioButton::toggled, ui->fileGroupBox, &QGroupBox::setVisible);

	connect(ui->standardRadioButton, &QRadioButton::toggled, this, &ImportDialog::handleModeChange);

	connect(ui->fileEdit, &FilenameEditWidget::changed, this, &ImportDialog::handleFileChange);
	connect(&m_selectionModel, &QItemSelectionModel::currentChanged, [=](const QModelIndex& cur, const QModelIndex&) {
		auto path = m_standardsModel.data(cur, PathRole).toString();
		handleFileChange(path);
	});

	connect(ui->directionCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->titleGroupCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->titleCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->titleFontSettingCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->labelDisplayGroupCheckBox , &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorGroupCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorTitleCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorLabelCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorBackgroundCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->sizeAndPositionGroupCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->sizeCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->positionCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->barWidthCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->alignmentCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);

	ui->fileEdit->setFilter(tr("Enumerate colormap setting (*.ecmsetting)"));

	loadStandards();
	handleFileChange("");

	Setting::get(this);
}

ColorMapEnumerateSettingEditWidget::ImportDialog::~ImportDialog()
{
	Setting::set(this);

	delete ui;
}

const ColorMapEnumerateSettingContainer& ColorMapEnumerateSettingEditWidget::ImportDialog::setting() const
{
	return m_setting;
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::setOriginalSetting(const ColorMapEnumerateSettingContainer& setting)
{
	m_originalSetting = setting;
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::accept()
{
	bool ok = importData();
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while importing setting from %1.").arg(QDir::toNativeSeparators(m_selectedFileName)));
		return;
	}

	QDialog::accept();
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::handleModeChange()
{
	if (ui->standardRadioButton->isChecked()) {
		auto index = m_selectionModel.currentIndex();
		handleFileChange(m_standardsModel.data(index, PathRole).toString());
	} else if (ui->fileRadioButton->isChecked()) {
		handleFileChange(ui->fileEdit->filename());
	}
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::handleFileChange(const QString& fileName)
{
	QFileInfo finfo(fileName);
	bool ok = (finfo.exists() && finfo.suffix() == "ecmsetting");

	ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(ok);
	m_selectedFileName = fileName;

	updatePreview();
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::updatePreview()
{
	bool ok = ok = importData();
	if (! ok) {
		ui->previewWidget->hide();
		return;
	}

	ui->previewWidget->show();

	BoolContainer trueContainer("visibility");
	trueContainer = true;
	ValueChangerT<BoolContainer> visibleChanger(&m_setting.legend.visible, trueContainer);

	QImage image = m_setting.legend.imageSetting.buildImage(this);
	ui->previewWidget->setImage(image);
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::clearPreview()
{
	ui->previewWidget->hide();
}

bool ColorMapEnumerateSettingEditWidget::ImportDialog::importData()
{
	bool ok = m_setting.importData(m_selectedFileName);
	if (! ok) {return false;}

	m_setting.valueCaption = m_originalSetting.valueCaption;
	m_setting.valueCaptions = m_originalSetting.valueCaptions;

	auto colors = m_originalSetting.colors;
	int minSize = colors.size();
	if (m_setting.colors.size() < minSize) {
		minSize = m_setting.colors.size();
	}
	for (int i = 0; i < minSize; ++i) {
		colors.at(i).color = m_setting.colors.at(i).color;
		colors.at(i).transparent = m_setting.colors.at(i).transparent;
	}
	m_setting.colors = colors;

	if (! ui->directionCheckBox->isChecked()) {
		m_setting.legend.direction = m_originalSetting.legend.direction;
	}
	if (! ui->titleGroupCheckBox->isChecked()) {
		if (! ui->titleCheckBox->isChecked()) {
			m_setting.legend.title = m_originalSetting.legend.title;
		}
		if (! ui->titleFontSettingCheckBox->isChecked()) {
			m_setting.legend.titleFont = m_originalSetting.legend.titleFont;
		}
	}
	if (! ui->labelDisplayGroupCheckBox->isChecked()) {
		m_setting.legend.labelFont = m_originalSetting.legend.labelFont;
	}
	if (! ui->colorGroupCheckBox->isChecked()) {
		if (! ui->colorTitleCheckBox->isChecked()) {
			m_setting.legend.titleColor = m_originalSetting.legend.titleColor;
		}
		if (! ui->colorLabelCheckBox->isChecked()) {
			m_setting.legend.labelColor = m_originalSetting.legend.labelColor;
		}
		if (! ui->colorBackgroundCheckBox->isChecked()) {
			m_setting.legend.backgroundColor = m_originalSetting.legend.backgroundColor;
			m_setting.legend.backgroundOpacity = m_originalSetting.legend.backgroundOpacity;
		}
	}
	if (! ui->sizeAndPositionGroupCheckBox->isChecked()) {
		if (! ui->sizeCheckBox->isChecked()) {
			m_setting.legend.imageSetting.width = m_originalSetting.legend.imageSetting.width;
			m_setting.legend.imageSetting.height = m_originalSetting.legend.imageSetting.height;
		}
		if (! ui->positionCheckBox->isChecked()) {
			m_setting.legend.imageSetting.horizontalMargin = m_originalSetting.legend.imageSetting.horizontalMargin;
			m_setting.legend.imageSetting.verticalMargin = m_originalSetting.legend.imageSetting.verticalMargin;
			m_setting.legend.imageSetting.position = m_originalSetting.legend.imageSetting.position;
		}
	}
	if (! ui->barWidthCheckBox->isChecked()) {
		m_setting.legend.barAutoWidth = m_originalSetting.legend.barAutoWidth;
		m_setting.legend.barWidth = m_originalSetting.legend.barWidth;
	}
	if (! ui->alignmentCheckBox->isChecked()) {
		m_setting.legend.barAlign = m_originalSetting.legend.barAlign;
	}

	return true;
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::loadStandards()
{
	QDir dir(iRICRootPath::get());
	dir.cdUp();
	dir.cdUp();

	ui->standardsTreeView->setModel(&m_standardsModel);
	ui->standardsTreeView->setSelectionModel(&m_selectionModel);

	auto publicPath = dir.absoluteFilePath("colormaps");
	auto publicFolder = new QStandardItem(QIcon(":/libs/guibase/images/iconFolder.svg"), tr("Public (%1)").arg(QDir::toNativeSeparators(publicPath)));
	m_standardsModel.insertRow(0, publicFolder);
	loadModelRecursive(publicPath, publicFolder);

	auto privatePath = dir.absoluteFilePath("private/colormaps");
	auto privateFolder = new QStandardItem(QIcon(":/libs/guibase/images/iconFolder.svg"), tr("Private (%1)").arg(QDir::toNativeSeparators(privatePath)));
	m_standardsModel.insertRow(1, privateFolder);
	loadModelRecursive(dir.absoluteFilePath("private/colormaps"), privateFolder);

	ui->standardsTreeView->expandAll();
}
