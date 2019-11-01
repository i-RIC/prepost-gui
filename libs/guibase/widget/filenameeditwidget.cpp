#include "ui_filenameeditwidget.h"

#include "filenameeditwidget.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

namespace {

QString getRelativePath(const QString& filename, const QString& baseFolder)
{
	QFileInfo finfo(filename);
	if (finfo.isRelative()) {return filename;}

	if (baseFolder.isEmpty()) {return filename;}

	if (filename.left(baseFolder.length()) != baseFolder) {return filename;}

	return filename.right(filename.length() - baseFolder.length() - 1);
}

} // namespace

FilenameEditWidget::FilenameEditWidget(QWidget* parent) :
	QWidget(parent),
	m_filter {tr("All files (*.*)")},
	m_saveMode {false},
	ui(new Ui::FilenameEditWidget)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openDialog()));
}

FilenameEditWidget::~FilenameEditWidget()
{
	delete ui;
}

QString FilenameEditWidget::filename() const
{
	return QDir::fromNativeSeparators(ui->lineEdit->text().trimmed());
}

void FilenameEditWidget::setFilename(const QString& filename)
{
	QString relativePath = getRelativePath(filename, m_baseFolder);

	ui->lineEdit->setText(QDir::toNativeSeparators(relativePath));
}

void FilenameEditWidget::setFilter(const QString& filter)
{
	m_filter = filter;
}

void FilenameEditWidget::setBaseFolder(const QString& folder)
{
	m_baseFolder = folder;
}

void FilenameEditWidget::setSaveMode(bool saveMode)
{
	m_saveMode = saveMode;
}

void FilenameEditWidget::openDialog()
{
	QFileInfo finfo(ui->lineEdit->text());
	QString dir = finfo.absolutePath();

	QString fname;
	if (m_saveMode) {
		fname = QFileDialog::getSaveFileName(this, tr("Select File"), dir, m_filter);
	} else {
		fname = QFileDialog::getOpenFileName(this, tr("Select File"), dir, m_filter);
	}

	if (fname.isNull()) {return;}

	ui->lineEdit->setText(getRelativePath(fname, m_baseFolder));
}
