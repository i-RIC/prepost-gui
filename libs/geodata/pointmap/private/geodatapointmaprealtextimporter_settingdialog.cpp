#include "geodatapointmaprealtextimporter_importersetting.h"
#include "geodatapointmaprealtextimporter_lineparser.h"
#include "geodatapointmaprealtextimporter_settingdialog.h"
#include "ui_geodatapointmaprealtextimporter_settingdialog.h"

#include <QPushButton>
#include <QTextCodec>

#include <memory>

GeoDataPointmapRealTextImporter::SettingDialog::SettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapRealTextImporter_SettingDialog)
{
	ui->setupUi(this);

	connect(ui->csvRadioButton, &QRadioButton::toggled, this, &SettingDialog::csvToggled);
	connect<void (QSpinBox::*)(int)>(ui->headerLinesSpinBox, &QSpinBox::valueChanged, this, &SettingDialog::headerLinesChange);
	connect<void (QComboBox::*)(int)>(ui->encodingComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::updatePreview);
	connect(ui->csvRadioButton, &QRadioButton::toggled, this, &SettingDialog::updatePreview);
	connect(ui->commaCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->tabCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->spaceCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->colonCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->semicolonCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->otherCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect(ui->otherLineEdit, &QLineEdit::textChanged, this, &SettingDialog::updatePreview);
	connect(ui->quoteLineEdit, &QLineEdit::textChanged, this, &SettingDialog::updatePreview);
	connect(ui->escapeLineEdit, &QLineEdit::textChanged, this, &SettingDialog::updatePreview);
	connect<void (QSpinBox::*)(int)>(ui->headerLinesSpinBox, &QSpinBox::valueChanged, this, &SettingDialog::updatePreview);
	connect(ui->fieldNameCheckBox, &QCheckBox::toggled, this, &SettingDialog::updatePreview);
	connect<void (QComboBox::*)(int)>(ui->xFieldComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::updatePreview);
	connect<void (QComboBox::*)(int)>(ui->yFieldComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::updatePreview);
	connect<void (QComboBox::*)(int)>(ui->valueFieldComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::updatePreview);
	connect<void (QSpinBox::*)(int)>(ui->skipRateSpinBox, &QSpinBox::valueChanged, this, &SettingDialog::updatePreview);

	QList<QByteArray> codecs = QTextCodec::availableCodecs();
	qSort(codecs);
	for (const auto& name : codecs) {
		ui->encodingComboBox->addItem(QString(name));
	}
	QTextCodec* defaultCodec = QTextCodec::codecForLocale();
	int index = codecs.indexOf(defaultCodec->name());
	ui->encodingComboBox->setCurrentIndex(index);

	QList<int> sizes;
	sizes << 1 << 10000000;
	ui->splitter->setSizes(sizes);
}

GeoDataPointmapRealTextImporter::SettingDialog::~SettingDialog()
{
	delete ui;
}

void GeoDataPointmapRealTextImporter::SettingDialog::setFileName(const QString& fileName)
{
	ui->fileNameValueLabel->setText(fileName);
}

void GeoDataPointmapRealTextImporter::SettingDialog::setIsCsv(bool isCsv)
{
	if (isCsv) {
		ui->csvRadioButton->setChecked(true);
	} else {
		ui->customRadioButton->setChecked(true);
	}
}

void GeoDataPointmapRealTextImporter::SettingDialog::setPreviewData(const std::vector<QByteArray>& data)
{
	m_previewData = data;

	autoDetectDelimiters();
	autoDetectHeaderLines();

	updatePreview();
}

void GeoDataPointmapRealTextImporter::SettingDialog::setBuilder(CoordinateSystemBuilder* builder)
{
	return ui->csWidget->setBuilder(builder);
}

void GeoDataPointmapRealTextImporter::SettingDialog::setCsEnabled(bool enabled)
{
	return ui->csWidget->setEnabled(enabled);
}

CoordinateSystem* GeoDataPointmapRealTextImporter::SettingDialog::coordinateSystem() const
{
	return ui->csWidget->coordinateSystem();
}

void GeoDataPointmapRealTextImporter::SettingDialog::setCoordinateSystem(CoordinateSystem* cs)
{
	ui->csWidget->setCoordinateSystem(cs);
}

void GeoDataPointmapRealTextImporter::SettingDialog::updateComboBoxes()
{
	std::vector<QComboBox*> comboBoxes;
	std::vector<int> oldSels;

	comboBoxes.push_back(ui->xFieldComboBox);
	comboBoxes.push_back(ui->yFieldComboBox);
	comboBoxes.push_back(ui->valueFieldComboBox);

	for (auto c : comboBoxes) {
		c->blockSignals(true);
		oldSels.push_back(c->currentIndex());
	}

	bool ok;
	QString error;
	setupImporterSetting(&m_importerSetting, &ok, &error);
	if (! ok) {
		for (QComboBox* c : comboBoxes) {
			c->clear();
			c->setDisabled(true);
			c->blockSignals(false);
		}
		return;
	}

	std::unique_ptr<LineParser> parser(m_importerSetting.buildParser());

	QStringList fields = getFields(parser.get());

	for (int i = 0; i < static_cast<int> (comboBoxes.size()); ++i) {
		QComboBox* c = comboBoxes.at(i);
		int oldSel = oldSels.at(i);
		c->clear();
		for (const auto& f : fields) {
			c->addItem(f);
		}
		c->setEnabled(true);
		if (oldSel == -1) {
			oldSel = i;
		}
		c->setCurrentIndex(oldSel);
		c->blockSignals(false);
	}
}

void GeoDataPointmapRealTextImporter::SettingDialog::setupImporterSetting(ImporterSetting* setting, bool* ok, QString* error) const
{
	*ok = true;
	*error = "";

	setting->codecName = ui->encodingComboBox->currentText();

	bool delimiterSpecified = false;

	setting->delimiterComma = ui->commaCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterComma;

	setting->delimiterTab = ui->tabCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterTab;

	setting->delimiterSpace = ui->spaceCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterSpace;

	setting->delimiterColon = ui->colonCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterColon;

	setting->delimiterSemicolon = ui->semicolonCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterSemicolon;

	setting->delimiterOther = ui->otherCheckBox->isChecked();
	delimiterSpecified = delimiterSpecified || setting->delimiterOther;

	setting->otherDelimiter = ui->otherLineEdit->text();

	if (! delimiterSpecified) {
		*ok = false;
		*error = GeoDataPointmapRealTextImporter::tr("No delimiter specified");
		return;
	}

	setting->quoteCharacter = ui->quoteLineEdit->text();
	setting->escapeCharacter = ui->escapeLineEdit->text();

	setting->headerLines = ui->headerLinesSpinBox->value();
	setting->fieldX = ui->xFieldComboBox->currentIndex();
	setting->fieldY = ui->yFieldComboBox->currentIndex();
	setting->fieldValue = ui->valueFieldComboBox->currentIndex();
	setting->skipRate = ui->skipRateSpinBox->value();
}

void GeoDataPointmapRealTextImporter::SettingDialog::csvToggled(bool toggled)
{
	std::vector<QCheckBox*> delim_checkboxes;
	delim_checkboxes.push_back(ui->commaCheckBox);
	delim_checkboxes.push_back(ui->tabCheckBox);
	delim_checkboxes.push_back(ui->spaceCheckBox);
	delim_checkboxes.push_back(ui->colonCheckBox);
	delim_checkboxes.push_back(ui->semicolonCheckBox);
	delim_checkboxes.push_back(ui->otherCheckBox);

	if (toggled) {
		for (QCheckBox* c : delim_checkboxes) {
			c->setChecked(false);
			c->setEnabled(false);
		}
		ui->commaCheckBox->setChecked(true);
	} else {
		for (QCheckBox* c : delim_checkboxes) {
			c->setEnabled(true);
		}
	}
}

void GeoDataPointmapRealTextImporter::SettingDialog::headerLinesChange(int lines)
{
	ui->fieldNameCheckBox->setEnabled(lines > 0);
	if (lines == 0) {
		ui->fieldNameCheckBox->setChecked(false);
	}
}

void GeoDataPointmapRealTextImporter::SettingDialog::updatePreview()
{
	updateComboBoxes();

	ui->tableWidget->clear();

	bool ok;
	QString error;

	ui->errorsLabel->setText("");

	setupImporterSetting(&m_importerSetting, &ok, &error);
	if (! ok) {
		showErrorMessageAndDisableOkButton(error);
		return;
	}

	std::unique_ptr<LineParser> parser(m_importerSetting.buildParser());

	auto fields = getFields(parser.get());
	auto lines = getDataLinesForPreview();

	ui->tableWidget->setColumnCount(fields.size());
	ui->tableWidget->setRowCount(lines.size());

	ui->tableWidget->setHorizontalHeaderLabels(fields);

	if (fields.size() < 3) {
		showErrorMessageAndDisableOkButton(GeoDataPointmapRealTextImporter::tr("The number of fields is less than 3."));
	}

	if (ui->xFieldComboBox->currentIndex() == ui->yFieldComboBox->currentIndex() ||
			ui->xFieldComboBox->currentIndex() == ui->valueFieldComboBox->currentIndex() ||
			ui->yFieldComboBox->currentIndex() == ui->valueFieldComboBox->currentIndex()) {
		showErrorMessageAndDisableOkButton(GeoDataPointmapRealTextImporter::tr("X, Y and Value should refer to different fields."));
	}

	for (int i = 0; i < lines.size(); ++i) {
		const auto& line = lines.at(i);
		QString lineStr = parser->textCodec()->toUnicode(line);

		auto frags = parser->parseToStrs(lineStr, &ok, &error);
		if (! ok) {
			int lineNo = ui->headerLinesSpinBox->value() + i * (1 + ui->skipRateSpinBox->value()) + 1;
			showErrorMessageAndDisableOkButton(GeoDataPointmapRealTextImporter::tr("Line %1: %2").arg(lineNo).arg(error));
		}
		int vals = qMin(frags.size(), fields.size());
		for (int j = 0; j < vals; ++j) {
			auto item = new QTableWidgetItem(frags.at(j));
			ui->tableWidget->setItem(i, j, item);
		}
	}

	if (ui->errorsLabel->text().isEmpty()) {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}


void GeoDataPointmapRealTextImporter::SettingDialog::autoDetectHeaderLines()
{
	if (m_previewData.size() == 0) {return;}

	std::vector<int> numColumns;

	bool ok;
	QString	error;
	setupImporterSetting(&m_importerSetting, &ok, &error);
	if (! ok) {return;}

	std::unique_ptr<LineParser> parser(m_importerSetting.buildParser());

	for (int i = 0; i < m_previewData.size(); ++i) {
		auto line = QString(m_previewData.at(i));
		auto strs = parser->parseToStrs(line, &ok, &error);
		if (! ok) {
			numColumns.push_back(0);
		} else {
			numColumns.push_back(strs.length());
		}
	}
	int lastNumColumns = numColumns.at(numColumns.size() - 1);
	for (int l = m_previewData.size() - 2; l >= 0; --l) {
		if (numColumns.at(l) != lastNumColumns) {
			ui->headerLinesSpinBox->setValue(l + 1);
		}
	}
}

void GeoDataPointmapRealTextImporter::SettingDialog::autoDetectDelimiters()
{
	if (ui->csvRadioButton->isChecked()) {return;}
	if (m_previewData.size() == 0) {return;}

	QString firstLine = QString(m_previewData.at(0));

	if (firstLine.contains(",")) {ui->commaCheckBox->setChecked(true);}
	if (firstLine.contains("\t")) {ui->tabCheckBox->setChecked(true);}
	if (firstLine.contains(" ")) {ui->spaceCheckBox->setChecked(true);}
	if (firstLine.contains(":")) {ui->colonCheckBox->setChecked(true);}
	if (firstLine.contains(";")) {ui->semicolonCheckBox->setChecked(true);}
}

QStringList GeoDataPointmapRealTextImporter::SettingDialog::getFields(LineParser *parser) const
{

	bool ok;
	QString error;
	QString line;

	if (ui->fieldNameCheckBox->isChecked()) {
		if (m_previewData.size() > 0) {
			line = m_previewData.at(0);
		}
		return parser->parseToStrs(line, &ok, &error);
	} else {
		if (m_previewData.size() > ui->headerLinesSpinBox->value())
		line = m_previewData.at(ui->headerLinesSpinBox->value());
		auto frags = parser->parseToStrs(line, &ok, &error);

		QStringList fields;
		for (int i = 0; i < frags.size(); ++i) {
			fields.append(QString("field%1").arg(i + 1));
		}
		return fields;
	}
}

std::vector<QByteArray> GeoDataPointmapRealTextImporter::SettingDialog::getDataLinesForPreview() const
{
	std::vector<QByteArray> ret;
	int headers = ui->headerLinesSpinBox->value();
	int row = headers;
	int skipRate = ui->skipRateSpinBox->value();
	while (row < m_previewData.size()) {
		if ((row - headers) % skipRate == 0) {
			ret.push_back(m_previewData.at(row));
		}
		++ row;
	}
	return ret;
}

void GeoDataPointmapRealTextImporter::SettingDialog::showErrorMessageAndDisableOkButton(const QString& message)
{
	ui->errorsLabel->setText(message);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
