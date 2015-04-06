#ifndef PREPROCESSORGRIDRELATEDCONDITIONCELLDISPLAYSETTINGDIALOG_H
#define PREPROCESSORGRIDRELATEDCONDITIONCELLDISPLAYSETTINGDIALOG_H

#include <QDialog>
class QTableWidgetItem;
class QItemSelection;;

namespace Ui {
    class PreProcessorGridRelatedConditionCellDisplaySettingDialog;
}

class PreProcessorGridRelatedConditionCellDisplaySettingDialog : public QDialog
{
    Q_OBJECT

public:
	static const int ATTRWIDTH = 170;
	static const int COLORWIDTH = 50;
	explicit PreProcessorGridRelatedConditionCellDisplaySettingDialog(QWidget *parent = 0);
    ~PreProcessorGridRelatedConditionCellDisplaySettingDialog();

	void setupDialog();
private slots:
	void handleItemClick(QTableWidgetItem*);
	void handleItemEdit(QTableWidgetItem*);
	void handleShownSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);
private:
    Ui::PreProcessorGridRelatedConditionCellDisplaySettingDialog *ui;


public:
/*
	void setGridType(SolverDefinitionGridType* gt){m_gridType = gt;}
	void setSettings(const QList<Post2dWindowCellFlagSetting>& settings){
		m_settings = settings;
		setupDialog();
	}
	const QList<Post2dWindowCellFlagSetting>& settings();
*/
private:
//	QList<Post2dWindowCellFlagSetting> m_settings;
};

#endif // PREPROCESSORGRIDRELATEDCONDITIONCELLDISPLAYSETTINGDIALOG_H
