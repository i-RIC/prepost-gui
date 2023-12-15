#ifndef ABSTRACTCROSSSECTIONWINDOW_CONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_CONTROLLER_H

#include "../abstractcrosssectionwindow.h"

#include <QWidget>

namespace Ui {
class AbstractCrosssectionWindow_Controller;
}

class AbstractCrosssectionWindow::Controller : public QWidget
{
	Q_OBJECT

public:
	enum CellSide {
		Previous,
		Next
	};

	Controller(AbstractCrosssectionWindow* window, QWidget *parent = nullptr);
	~Controller();

	void setTarget(Direction dir, int index);
	Direction targetDirection() const;
	int targetIndex() const;
	int maxIndex() const;

	CellSide cellSide() const;
	void setCellSide(CellSide side);

signals:
	void snapshotClicked();
	void csvExportClicked();

private slots:
	void handleIClick();
	void handleJClick();
	void handleSettingChange();

private:
	void updateTargetRange();

	AbstractCrosssectionWindow* m_window;

	Ui::AbstractCrosssectionWindow_Controller *ui;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_CONTROLLER_H
