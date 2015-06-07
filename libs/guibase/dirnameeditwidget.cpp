#include "ui_dirnameeditwidget.h"

#include "dirnameeditwidget.h"

#include <QDir>
#include <QFileDialog>

DirnameEditWidget::DirnameEditWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::DirnameEditWidget)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openDialog()));
}

DirnameEditWidget::~DirnameEditWidget()
{
	delete ui;
}

void DirnameEditWidget::setDirname(const QString& dirname)
{
	ui->lineEdit->setText(QDir::toNativeSeparators(dirname));
}

const QString DirnameEditWidget::dirname() const
{
	return QDir::fromNativeSeparators(ui->lineEdit->text());
}

void DirnameEditWidget::openDialog()
{
	QString dir = QDir::fromNativeSeparators(ui->lineEdit->text());
	dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), dir);
	if (! dir.isNull()) {
		ui->lineEdit->setText(QDir::toNativeSeparators(dir));
	}
}
