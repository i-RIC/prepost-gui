#include "ui_filenameeditwidget.h"

#include "filenameeditwidget.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

FilenameEditWidget::FilenameEditWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::FilenameEditWidget)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openDialog()));
	m_filter = tr("All files (*.*)");
}

FilenameEditWidget::~FilenameEditWidget()
{
	delete ui;
}

void FilenameEditWidget::setFilename(const QString& filename)
{
	QFileInfo finfo(filename);
	QString fname = filename;
	if (finfo.isAbsolute() &&
			! m_baseDir.isNull() &&
			filename.left(m_baseDir.length()) == m_baseDir) {
		// store relative path.
		fname = filename.right(filename.length() - m_baseDir.length() - 1);
	}
	ui->lineEdit->setText(QDir::toNativeSeparators(fname));
}

void FilenameEditWidget::setFilter(const QString& filter)
{
	m_filter = filter;
}

QString FilenameEditWidget::filename() const
{
	return QDir::fromNativeSeparators(ui->lineEdit->text().trimmed());
}

void FilenameEditWidget::openDialog()
{
	QFileInfo finfo(ui->lineEdit->text());
	QString dir = finfo.absolutePath();
	if (! QFile::exists(dir)) {
		// @todo if dir name that does not exists specified, set
		// appropriate default directory here.
	}
	QString fname = QFileDialog::getOpenFileName(this, tr("Select File"), dir, m_filter);
	if (! fname.isNull()) {
		if (! m_baseDir.isNull() &&
				fname.left(m_baseDir.length()) == m_baseDir) {
			fname = fname.right(fname.length() - m_baseDir.length() - 1);
		}
		ui->lineEdit->setText(fname);
	}
}
