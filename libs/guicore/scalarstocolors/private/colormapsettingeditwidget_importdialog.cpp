#include "colormapsettingeditwidget_importdialog.h"
#include "colormapsettingeditwidget_importdialog_setting.h"
#include "ui_colormapsettingeditwidget_importdialog.h"

#include <misc/errormessage.h>
#include <misc/iricrootpath.h>
#include <misc/valuechangert.h>
#include <misc/xmlsupport.h>

#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QSize>
#include <QStandardItem>

namespace {

int PathRole = Qt::UserRole + 1;

QPixmap buildColormapIcon(const QString& filename)
{
	QPixmap defaultPixmap = QPixmap(":/libs/guibase/images/iconPaper.svg");
	
	if (filename.isNull()) { return defaultPixmap; }

	QFile f(filename);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (!ok) { return defaultPixmap; }
	auto node = doc.documentElement();
	
	
	std::vector<std::pair<double, QColor>> pairs;
	double maxValue = 0; 
	for (int i = 0; i < node.childNodes().size(); ++i) {
		auto childNode = node.childNodes().at(i);
		if (childNode.nodeName() == "Item") {
			const double value = iRIC::getDoubleAttribute(childNode, "value");
			const QColor color = iRIC::getColorAttribute(childNode, "color");
			pairs.push_back(std::make_pair(value, color));
			if (maxValue < std::abs(value)) {
				maxValue = value;
			}
		}
	}
	if (pairs.size() <= 1) { return defaultPixmap; }

	const QSize size(32, 32);
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter;
	painter.begin(&pixmap);

	for (int i = 0; i < pairs.size() - 1; i++) {
		// pair = (value, color)
		auto left = pairs.at(i).first / maxValue * size.width();
		auto right = pairs.at(i + 1).first / maxValue * size.width();
		auto colStart = pairs.at(i).second;
		auto colEnd = pairs.at(i + 1).second;

		QLinearGradient gradient(QPointF(left, 0), QPointF(right, 0));
		gradient.setColorAt(0, colStart);
		gradient.setColorAt(1, colEnd);
		QBrush brush(gradient);

		QRectF rect(left, 0, right - left, size.height());
		painter.fillRect(rect, brush);
	}
	
	painter.end();
	return pixmap;
}

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
		if (finfo.suffix() != "cmsetting") {continue;}

		auto item = new QStandardItem(QIcon(buildColormapIcon(finfo.absoluteFilePath())), finfo.fileName());
		item->setData(finfo.absoluteFilePath(), PathRole);
		parent->appendRow(item);
		childExists = true;
	}

	return childExists;
}

} // namespace

ColorMapSettingEditWidget::ImportDialog::ImportDialog(QWidget *parent) :
	QDialog(parent),
	m_selectionModel(&m_standardsModel),
	ui(new Ui::ColorMapSettingEditWidget_ImportDialog)
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
	connect(ui->labelFormatCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->numLabelsCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->labelFontSettingCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorGroupCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorTitleCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorLabelCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->colorBackgroundCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->sizeAndPositionGroupCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->sizeCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->positionCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->barWidthCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);
	connect(ui->alignmentCheckBox, &QCheckBox::toggled, this, &ImportDialog::updatePreview);

	ui->fileEdit->setFilter(tr("Colormap setting (*.cmsetting)"));

	loadStandards();
	handleFileChange("");

	Setting::get(this);
}

ColorMapSettingEditWidget::ImportDialog::~ImportDialog()
{
	Setting::set(this);

	delete ui;
}

const ColorMapSettingContainer& ColorMapSettingEditWidget::ImportDialog::setting() const
{
	return m_setting;
}

void ColorMapSettingEditWidget::ImportDialog::setOriginalSetting(const ColorMapSettingContainer& setting)
{
	m_originalSetting = setting;
}

void ColorMapSettingEditWidget::ImportDialog::accept()
{
	bool ok = importData();
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while importing setting from %1.").arg(QDir::toNativeSeparators(m_selectedFileName)));
		return;
	}

	QDialog::accept();
}

void ColorMapSettingEditWidget::ImportDialog::handleModeChange()
{
	if (ui->standardRadioButton->isChecked()) {
		auto index = m_selectionModel.currentIndex();
		handleFileChange(m_standardsModel.data(index, PathRole).toString());
	} else if (ui->fileRadioButton->isChecked()) {
		handleFileChange(ui->fileEdit->filename());
	}
}

void ColorMapSettingEditWidget::ImportDialog::handleFileChange(const QString& fileName)
{
	QFileInfo finfo(fileName);
	bool ok = (finfo.exists() && finfo.suffix() == "cmsetting");

	ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(ok);
	m_selectedFileName = fileName;

	updatePreview();
}

void ColorMapSettingEditWidget::ImportDialog::updatePreview()
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

void ColorMapSettingEditWidget::ImportDialog::clearPreview()
{
	ui->previewWidget->hide();
}

bool ColorMapSettingEditWidget::ImportDialog::importData()
{
	bool ok = m_setting.importData(m_selectedFileName);
	if (! ok) {return false;}

	m_setting.setAutoValueRange(m_originalSetting.autoMinValue, m_originalSetting.autoMaxValue);

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
		if (! ui->labelFormatCheckBox->isChecked()) {
			m_setting.legend.labelFormat = m_originalSetting.legend.labelFormat;
		}
		if (! ui->numLabelsCheckBox->isChecked()) {
			m_setting.legend.autoNumberOfLabels = m_originalSetting.legend.autoNumberOfLabels;
			m_setting.legend.numberOfLabels = m_originalSetting.legend.numberOfLabels;
		}
		if (! ui->labelFontSettingCheckBox->isChecked()) {
			m_setting.legend.labelFont = m_originalSetting.legend.labelFont;
		}
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

void ColorMapSettingEditWidget::ImportDialog::loadStandards()
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
