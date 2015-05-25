#ifndef GRAPH2DHYBRIDWINDOWDATASOURCEDIALOG_H
#define GRAPH2DHYBRIDWINDOWDATASOURCEDIALOG_H

#include "graph2dhybridwindowresultsetting.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class Graph2dHybridWindowDataSourceDialog;
}

class iRICMainWindowInterface;
class Graph2dHybridWindowResultSetting;
class QListWidget;
class Graph2dHybridWindowImportDataGroupDataItem;
class Graph2dHybridWindowImportDataDataItem;

class Graph2dHybridWindowDataSourceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindowDataSourceDialog(QWidget* parent = nullptr);
	~Graph2dHybridWindowDataSourceDialog();
	void setSetting(const Graph2dHybridWindowResultSetting& setting);
	void setImportData(Graph2dHybridWindowImportDataGroupDataItem* group);
	const Graph2dHybridWindowResultSetting& setting();
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
	QList<Graph2dHybridWindowResultSetting::XAxisMode> m_xAxisModes;
	Graph2dHybridWindowResultSetting m_setting;

	QList<Graph2dHybridWindowImportDataDataItem*> m_data;
	QList<Graph2dHybridWindowImportDataDataItem*> m_addedData;
	QList<Graph2dHybridWindowImportDataDataItem*> m_deletedData;

	Graph2dHybridWindowImportDataGroupDataItem* m_importDataGroup;

	iRICMainWindowInterface* m_mainWindow;

	Ui::Graph2dHybridWindowDataSourceDialog* ui;
};

#endif // GRAPH2DHYBRIDWINDOWDATASOURCEDIALOG_H
