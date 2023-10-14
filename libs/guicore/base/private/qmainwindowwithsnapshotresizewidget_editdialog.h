#ifndef QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_EDITDIALOG_H
#define QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_EDITDIALOG_H

#include "../qmainwindowwithsnapshotresizewidget.h"

#include <misc/windowsizestandardsetting.h>

#include <QDialog>

namespace Ui {
class QMainWindowWithSnapshotResizeWidget_EditDialog;
}

class QMainWindowWithSnapshotResizeWidget::EditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EditDialog(QWidget *parent = nullptr);
	~EditDialog();

	QSize size() const;
	void setSize(const QSize& size);

private:
	std::vector<WindowSizeStandardSetting> m_settings;
	Ui::QMainWindowWithSnapshotResizeWidget_EditDialog *ui;
};

#endif // QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_EDITDIALOG_H
