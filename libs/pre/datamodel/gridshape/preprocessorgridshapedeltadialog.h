#ifndef PREPROCESSORGRIDSHAPEDELTADIALOG_H
#define PREPROCESSORGRIDSHAPEDELTADIALOG_H

#include <QDialog>
#include <QVector2D>

namespace Ui
{
	class PreProcessorGridShapeDeltaDialog;
}

class PreProcessorGridShapeDeltaDialog : public QDialog
{
	Q_OBJECT

public:
	PreProcessorGridShapeDeltaDialog(QWidget* parent = nullptr);
	~PreProcessorGridShapeDeltaDialog();

	QVector2D delta();

private:
	Ui::PreProcessorGridShapeDeltaDialog* ui;
};

#endif // PREPROCESSORGRIDSHAPEDELTADIALOG_H
