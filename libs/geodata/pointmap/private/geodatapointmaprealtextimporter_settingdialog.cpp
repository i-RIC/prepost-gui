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

	connect(ui->csvRadioButton, SIGNAL(toggled(bool)), this, SLOT(csvToggled(bool)));
	connect(ui->headerLinesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(headerLinesChange(int)));

	connect(ui->encodingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
	connect(ui->csvRadioButton, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->commaCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->tabCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->spaceCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->colonCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->semicolonCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->otherCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->otherLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui->quoteLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui->escapeLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updatePreview()));
	connect(ui->headerLinesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
	connect(ui->fieldNameCheckBox, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
	connect(ui->xFieldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
	connect(ui->yFieldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
	connect(ui->valueFieldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
	connect(ui->skipRateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));

	QList<QByteArray> codecs = QTextCodec::availableCodecs();
	qSort(codecs);
	for (const auto& name : codecs) {
		ui->encodingComboBox->addItem(QString(name));
	}
	QTextCodec* defaultCodec = QTextCodec::codecForLocale();
	int index = codecs.indexOf(defaultCodec->name());
	ui->encodingComboBox->setCurrentIndex(index);
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

	for (QComboBox* c : comboBoxes) {
		c->blockSignals(true);
		oldSels.push_back(c->currentIndex());
	}

	bool ok;
	QString error;

	std::unique_ptr<LineParser> parser(buildParser(&ok, &error));

	if (! ok) {
		for (QComboBox* c : comboBoxes) {
			c->clear();
			c->setDisabled(true);
			c->blockSignals(false);
		}
		return;
	}
	QStringList fields = getFields(parser.get());

	for (int i = 0; i < comboBoxes.size(); ++i) {
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

GeoDataPointmapRealTextImporter::LineParser* GeoDataPointmapRealTextImporter::SettingDialog::buildParser(bool *ok, QString *error) const
{
	*ok = true;
	*error = "";

	auto parser = new LineParser();

	auto codec = QTextCodec::codecForName(ui->encodingComboBox->currentText().toLatin1());
	parser->setTextCodec(codec);

	std::vector<QChar> delimiters;

	if (ui->commaCheckBox->isChecked()) {
		delimiters.push_back(',');
	}
	if (ui->tabCheckBox->isChecked()) {
		delimiters.push_back('\t');
	}
	if (ui->spaceCheckBox->isChecked()) {
		delimiters.push_back(' ');
	}
	if (ui->colonCheckBox->isChecked()) {
		delimiters.push_back(':');
	}
	if (ui->semicolonCheckBox->isChecked()) {
		delimiters.push_back(';');
	}
	if (ui->otherCheckBox->isChecked()) {
		auto other = ui->otherLineEdit->text();
		for (int i = 0; i < other.size(); ++i) {
			delimiters.push_back(other.at(i));
		}
	}
	if (delimiters.size() == 0) {
		*ok = false;
		*error = GeoDataPointmapRealTextImporter::tr("No delimiter specified");
		return parser;
	}
	parser->setDelimiters(delimiters);

	std::vector<QChar> quotes;
	auto quoteStr = ui->quoteLineEdit->text();
	for (int i = 0; i < quoteStr.size(); ++i) {
		quotes.push_back(quoteStr.at(i));
	}
	parser->setQuoteChars(quotes);

	QChar escape;
	auto escapeStr = ui->escapeLineEdit->text();
	if (escapeStr.size() > 0) {
		escape = escapeStr.at(0);
	}
	parser->setEscapeChar(escape);

	parser->setHeaderLines(ui->headerLinesSpinBox->value());
	parser->setIsFirstLineFieldNames(ui->fieldNameCheckBox->isChecked());
	parser->setXColumn(ui->xFieldComboBox->currentIndex());
	parser->setYColumn(ui->yFieldComboBox->currentIndex());
	parser->setValueColumn(ui->valueFieldComboBox->currentIndex());
	parser->setSkipRate(ui->skipRateSpinBox->value());

	return parser;
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

	std::unique_ptr<LineParser> parser(buildParser(&ok, &error));
	if (! ok) {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		ui->errorsLabel->setText(error);
	}

	auto fields = getFields(parser.get());
	auto lines = getDataLinesForPreview();

	ui->tableWidget->setColumnCount(fields.size());
	ui->tableWidget->setRowCount(lines.size());

	ui->tableWidget->setHorizontalHeaderLabels(fields);

	for (int i = 0; i < lines.size(); ++i) {
		const auto& line = lines.at(i);
		QString lineStr = parser->textCodec()->toUnicode(line);

		auto frags = parser->parseToStrs(lineStr, &ok, &error);
		if (! ok) {
			ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			int lineNo = ui->headerLinesSpinBox->value() + i * (1 + ui->skipRateSpinBox->value()) + 1;
			ui->errorsLabel->setText(GeoDataPointmapRealTextImporter::tr("Line %1: %2").arg(lineNo).arg(error));
			return;
		}
		int vals = qMin(frags.size(), fields.size());
		for (int j = 0; j < vals; ++j) {
			auto item = new QTableWidgetItem(frags.at(j));
			ui->tableWidget->setItem(i, j, item);
		}
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	ui->errorsLabel->setText("");
}


void GeoDataPointmapRealTextImporter::SettingDialog::autoDetectHeaderLines()
{
	if (m_previewData.size() == 0) {return;}

	std::vector<int> numColumns;

	bool ok;
	QString	error;
	std::unique_ptr<LineParser> parser(buildParser(&ok, &error));
	if (! ok) {return;}

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
