#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWDATASOURCEDIALOG_H
#define GRAPH2DVERIFICATIONWINDOWDATASOURCEDIALOG_H

#include "graph2dverificationwindowresultsetting.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class Graph2dVerificationWindowDataSourceDialog;
}

class iRICMainWindowInterface;
class Graph2dVerificationWindowResultSetting;
class QListWidget;
class Graph2dVerificationWindowImportDataGroupDataItem;
class Graph2dVerificationWindowImportDataDataItem;

class Graph2dVerificationWindowDataSourceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindowDataSourceDialog(QWidget* parent = nullptr);
	~Graph2dVerificationWindowDataSourceDialog();
	void setSetting(const Graph2dVerificationWindowResultSetting& setting);
	void setImportData(Graph2dVerificationWindowImportDataGroupDataItem* group);
	const Graph2dVerificationWindowResultSetting& setting();
	void setMainWindow(iRICMainWindowInterface* mainw) {m_mainWindow = mainw;}

public slots:
	void accept() override;
	void reject() override;

private slots:
	void changeAxis(int index);
	void changePointComboBox(int index);
	void changeOneDimComboBox(int index);
	void changeTwoDimComboBox(int index);
	void changeThreeDimComboBox(int index);

	void changeOneDimGridLocationComboBox(int index);
	void changeTwoDimGridLocationComboBox(int index);
	void changeThreeDimGridLocationComboBox(int index);

	void pointDataFocus();
	void oneDimDataFocus();
	void twoDimDataFocus();
	void threeDimDataFocus();
	void selectedFocus();

	void addSetting();
	void removeSetting();
	void editSetting();
	void importCsv();
	void deleteImportItem();

private:
	void setTargetDataTypeInfo();
	void clearTargetDataTypeInfo();
	void updateImportDataList();
	void updateButtonStatus();
	QListWidget* getActiveListWidget();
	void updateLists(QListWidget* widget);
	QList<Graph2dVerificationWindowResultSetting::XAxisMode> m_xAxisModes;
	Graph2dVerificationWindowResultSetting m_setting;

	QList<Graph2dVerificationWindowImportDataDataItem*> m_data;
	QList<Graph2dVerificationWindowImportDataDataItem*> m_addedData;
	QList<Graph2dVerificationWindowImportDataDataItem*> m_deletedData;

	Graph2dVerificationWindowImportDataGroupDataItem* m_importDataGroup;

	iRICMainWindowInterface* m_mainWindow;

	Ui::Graph2dVerificationWindowDataSourceDialog* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWDATASOURCEDIALOG_H
#endif
