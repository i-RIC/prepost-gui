#ifndef GRAPH2DSCATTEREDWINDOWDRAWSETTINGDIALOG_H
#define GRAPH2DSCATTEREDWINDOWDRAWSETTINGDIALOG_H

#include "graph2dscatteredwindowresultsetting.h"
#include <QDialog>

namespace Ui {
class Graph2dScatteredWindowDrawSettingDialog;
}

class Graph2dScatteredWindowDrawSettingDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit Graph2dScatteredWindowDrawSettingDialog(QWidget *parent = 0);
	~Graph2dScatteredWindowDrawSettingDialog();
	
	void setResultSettings(const QList<Graph2dScatteredWindowResultSetting::Setting>& list);

	const QList<Graph2dScatteredWindowResultSetting::Setting>& resultSettings() const {
		return m_resultSettings;
	}

private slots:
	void updateResultLabel(const QString& label);
	void setResultItem(int index);

private:
	int copyIndex(int index);
	int importDataIndex(int index);

	QList<Graph2dScatteredWindowResultSetting::Setting> m_resultSettings;

	Ui::Graph2dScatteredWindowDrawSettingDialog *ui;
};

#endif // GRAPH2DSCATTEREDWINDOWDRAWSETTINGDIALOG_H
