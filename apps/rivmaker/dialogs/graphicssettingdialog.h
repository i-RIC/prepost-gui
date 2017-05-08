#ifndef GRAPHICSSETTINGDIALOG_H
#define GRAPHICSSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GraphicsSettingDialog;
}

class GraphicsSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GraphicsSettingDialog(QWidget *parent = 0);
	~GraphicsSettingDialog();

public slots:
	void accept() override;

private:
	Ui::GraphicsSettingDialog *ui;
};

#endif // GRAPHICSSETTINGDIALOG_H
