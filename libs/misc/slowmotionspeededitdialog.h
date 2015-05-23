#ifndef SLOWMOTIONSPEEDEDITDIALOG_H
#define SLOWMOTIONSPEEDEDITDIALOG_H

#include "misc_global.h"
#include <QDialog>

namespace Ui
{
	class SlowmotionSpeedEditDialog;
}

/// Dialog to edit speed for slowmotion animation
class MISCDLL_EXPORT SlowmotionSpeedEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SlowmotionSpeedEditDialog(QWidget* parent = 0);
	~SlowmotionSpeedEditDialog();
	/// Set the interval between animation frames as seconds
	void setInterval(double interval);
	/// The interval between animation frames as seconds
	double interval();

private:
	Ui::SlowmotionSpeedEditDialog* ui;
};

#endif // SLOWMOTIONSPEEDEDITDIALOG_H
