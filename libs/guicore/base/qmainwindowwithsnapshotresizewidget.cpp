#include "qmainwindowwithsnapshotresizewidget.h"
#include "ui_qmainwindowwithsnapshotresizewidget.h"
#include "private/qmainwindowwithsnapshotresizewidget_editdialog.h"

#include <guicore/base/qmainwindowwithsnapshot.h>

QMainWindowWithSnapshotResizeWidget::QMainWindowWithSnapshotResizeWidget(QWidget *parent) :
	QWidget(parent),
	m_window {nullptr},
	ui(new Ui::QMainWindowWithSnapshotResizeWidget)
{
	ui->setupUi(this);
	connect(ui->sizeLabel, &QLabelWithClickedSignal::clicked, this, &QMainWindowWithSnapshotResizeWidget::editSize);
	updateDisplay();
}

QMainWindowWithSnapshotResizeWidget::~QMainWindowWithSnapshotResizeWidget()
{
	delete ui;
}

void QMainWindowWithSnapshotResizeWidget::setWindow(QMainWindowWithSnapshot* window)
{
	m_window = window;
	updateDisplay();

	connect(window, &QMainWindowWithSnapshot::snapshotAreaResized, this, &QMainWindowWithSnapshotResizeWidget::updateDisplay);
}

void QMainWindowWithSnapshotResizeWidget::updateDisplay()
{
	QString sizeStr = "---";

	if (m_window != nullptr) {
		auto s = m_window->snapshotArea()->size() * devicePixelRatioF();
		sizeStr = QString("%1 x %2").arg(static_cast<int>(s.width())).arg(static_cast<int>(s.height()));
	}

	ui->sizeLabel->setText(tr("Size: %1").arg(sizeStr));
}

void QMainWindowWithSnapshotResizeWidget::editSize()
{
	if (m_window == nullptr) {return;}

	auto s = m_window->snapshotArea()->size();
	auto s2 = s * devicePixelRatioF();

	EditDialog dialog(this);
	dialog.setSize(s2);

	auto ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto a = dynamic_cast<QMdiSubWindow*> (m_window->parent());
	m_window->resizeSnapshotAreaTo(dialog.size() / devicePixelRatioF(), a);
}
