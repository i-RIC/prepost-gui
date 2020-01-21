#include "geodatanetcdffilenamematcher.h"
#include "geodatanetcdffilenamepatterndialog.h"
#include "ui_geodatanetcdffilenamepatterndialog.h"

#include <QDateTime>
#include <QPushButton>

GeoDataNetcdfFileNamePatternDialog::GeoDataNetcdfFileNamePatternDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataNetcdfFileNamePatternDialog),
	m_matcher {nullptr}
{
	ui->setupUi(this);
	connect(ui->patternLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateMatcher()));
}

GeoDataNetcdfFileNamePatternDialog::~GeoDataNetcdfFileNamePatternDialog()
{
	delete ui;
	delete m_matcher;
}

void GeoDataNetcdfFileNamePatternDialog::setFilename(const QString& filename)
{
	m_filename = filename;
	ui->fileNameValueLabel->setText(filename);

	updateMatcher();
}

GeoDataNetcdfFileNameMatcher* GeoDataNetcdfFileNamePatternDialog::matcher()
{
	auto m = m_matcher;
	m_matcher = nullptr;
	return m;
}

void GeoDataNetcdfFileNamePatternDialog::updateMatcher()
{
	delete m_matcher;
	m_matcher = new GeoDataNetcdfFileNameMatcher(ui->patternLineEdit->text().trimmed());
	bool ok = m_matcher->setup(m_filename);
	if (! ok) {
		ui->resultValueLabel->setText(tr("File name does not match the pattern"));
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
	} else {
		bool ok;
		QDateTime dt = m_matcher->getDateTime(m_filename, &ok);
		ui->resultValueLabel->setText(dt.toString("yyyy/MM/dd HH:mm:ss"));
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}
