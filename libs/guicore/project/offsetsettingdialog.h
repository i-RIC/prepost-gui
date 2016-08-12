#ifndef OFFSETSETTINGDIALOG_H
#define OFFSETSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class OffsetSettingDialog;
}

class OffsetSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OffsetSettingDialog(QWidget* parent = nullptr);
	~OffsetSettingDialog();

	double offsetX() const;
	double offsetY() const;

	void setOffset(double x, double y);

public slots:
	void accept() override;

private:
	bool isPrecisionGood(double v);

	Ui::OffsetSettingDialog* ui;
};

#endif // OFFSETSETTINGDIALOG_H
