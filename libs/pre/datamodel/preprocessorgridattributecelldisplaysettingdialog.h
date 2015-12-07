#ifndef PREPROCESSORGRIDATTRIBUTECELLDISPLAYSETTINGDIALOG_H
#define PREPROCESSORGRIDATTRIBUTECELLDISPLAYSETTINGDIALOG_H

#include <QDialog>
class QTableWidgetItem;
class QItemSelection;

namespace Ui
{
	class PreProcessorGridAttributeCellDisplaySettingDialog;
}

class PreProcessorGridAttributeCellDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	static const int ATTRWIDTH = 170;
	static const int COLORWIDTH = 50;
	explicit PreProcessorGridAttributeCellDisplaySettingDialog(QWidget* parent = nullptr);
	~PreProcessorGridAttributeCellDisplaySettingDialog();

	void setupDialog();

private slots:
	void handleItemClick(QTableWidgetItem*);
	void handleItemEdit(QTableWidgetItem*);
	void handleShownSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

private:
	Ui::PreProcessorGridAttributeCellDisplaySettingDialog* ui;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLDISPLAYSETTINGDIALOG_H
