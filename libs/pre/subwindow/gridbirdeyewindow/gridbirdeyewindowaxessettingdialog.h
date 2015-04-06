#ifndef GRIDBIRDEYEWINDOWAXESSETTINGDIALOG_H
#define GRIDBIRDEYEWINDOWAXESSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
    class GridBirdEyeWindowAxesSettingDialog;
}

class GridBirdEyeWindowAxesSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridBirdEyeWindowAxesSettingDialog(QWidget *parent = 0);
    ~GridBirdEyeWindowAxesSettingDialog();
	void setAxesVisible(bool visible);
	bool axesVisible();
	void setColor(const QColor& col);
	QColor color();

private:
    Ui::GridBirdEyeWindowAxesSettingDialog *ui;
};

#endif // GRIDBIRDEYEWINDOWAXESSETTINGDIALOG_H
