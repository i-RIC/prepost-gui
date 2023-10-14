#ifndef QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_H
#define QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class QMainWindowWithSnapshotResizeWidget;
}

class QMainWindowWithSnapshot;

class GUICOREDLL_EXPORT QMainWindowWithSnapshotResizeWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QMainWindowWithSnapshotResizeWidget(QWidget *parent);
	~QMainWindowWithSnapshotResizeWidget();

	void setWindow(QMainWindowWithSnapshot* window);

private slots:
	void updateDisplay();
	void editSize();

private:
	QMainWindowWithSnapshot* m_window;
	Ui::QMainWindowWithSnapshotResizeWidget *ui;

	class EditDialog;
};

#endif // QMAINWINDOWWITHSNAPSHOTRESIZEWIDGET_H
