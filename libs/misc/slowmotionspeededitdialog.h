#ifndef SLOWMOTIONSPEEDEDITDIALOG_H
#define SLOWMOTIONSPEEDEDITDIALOG_H

#include "misc_global.h"
#include <QDialog>

namespace Ui {
	class SlowmotionSpeedEditDialog;
}

class MISCDLL_EXPORT SlowmotionSpeedEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SlowmotionSpeedEditDialog(QWidget *parent = 0);
	~SlowmotionSpeedEditDialog();
	void setInterval(double interval);
	double interval();
private:
	Ui::SlowmotionSpeedEditDialog *ui;
};

#endif // SLOWMOTIONSPEEDEDITDIALOG_H
