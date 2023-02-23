#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_CONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_CONTROLLER_H

#include "../preprocessorgridcrosssectionwindow2.h"

#include <QWidget>

namespace Ui {
class PreProcessorGridCrosssectionWindow2_Controller;
}

class PreProcessorGridCrosssectionWindow2::Controller : public QWidget
{
	Q_OBJECT

public:
	enum CellSide {
		Previous,
		Next
	};

	Controller(Impl* impl, QWidget *parent = nullptr);
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

	Impl* m_impl;

	Ui::PreProcessorGridCrosssectionWindow2_Controller *ui;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_CONTROLLER_H
