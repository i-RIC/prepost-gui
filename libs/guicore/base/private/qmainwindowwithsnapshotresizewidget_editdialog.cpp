#include "qmainwindowwithsnapshotresizewidget_editdialog.h"
#include "ui_qmainwindowwithsnapshotresizewidget_editdialog.h"

QMainWindowWithSnapshotResizeWidget::EditDialog::EditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QMainWindowWithSnapshotResizeWidget_EditDialog)
{
	ui->setupUi(this);
}

QMainWindowWithSnapshotResizeWidget::EditDialog::~EditDialog()
{
	delete ui;
}

QSize QMainWindowWithSnapshotResizeWidget::EditDialog::size() const
{
	return ui->widget->size();
}

void QMainWindowWithSnapshotResizeWidget::EditDialog::setSize(const QSize& size)
{
	ui->widget->setSize(size);
}
