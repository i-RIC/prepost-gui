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

class QComboBox;
class QLabel;

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
	void handleXAxisComboBoxChange(int index);
	void handlePolyLineGroupComboBoxChange(int index);

	void handlePointComboBoxChange(int index);
	void handleOneDimComboBoxChange(int index);
	void handleTwoDimComboBoxChange(int index);
	void handleThreeDimComboBoxChange(int index);

	void handleOneDimGridLocationComboBoxChange(int index);
	void handleTwoDimGridLocationComboBoxChange(int index);
	void handleThreeDimGridLocationComboBoxChange(int index);

	void handlePointDataListClick();
	void handleOneDimDataListClick();
	void handleTwoDimDataListClick();
	void handleThreeDimDataListClick();
	void handleSelectedDataListClick();

	void addSetting();
	void removeSetting();
	void editSetting();
	void importCsv();
	void deleteImportItem();

private:
	void setupXAxisComboBox();
	void setupPolyLineGroupComboBox();
	void updatePolyLineGroupComboBoxSelection();
	void updatePolyLineComboBox();
	void updatePolyLineComboBoxSelection();
	void updateSettingWidgets();
	void updatePolyLineComboBoxVisibility();

	void setupMainWidgets();
	void enableMainWidgets(bool enable);

	void setupWidgetForDim(QLabel* dataLabel, QComboBox* dataComboBox, QLabel* locationLabel, QComboBox* locationComboBox, QListWidget* dataListWidget, std::vector<std::string>* nameList, const QList<Graph2dHybridWindowResultSetting::DataTypeInfo*>& dataList);
	void handleDataComboBoxChange(int index, QComboBox* comboBox, QListWidget* dataListWidget, std::vector<std::string>* nameList, Graph2dHybridWindowResultSetting::DimType dimType);
	void handleLocationComboBoxChange(int index, QComboBox* locationComboBox, QComboBox* dataComboBox, QListWidget* dataListWidget, std::vector<std::string>* nameList, Graph2dHybridWindowResultSetting::DimType dimType);

	void clearListSelectionExcept(QListWidget* listWidget);

	void setupTargetDataTypeInfo();
	void clearTargetDataTypeInfo();
	void updateImportDataList();
	void updateButtonStatus();
	QListWidget* getActiveListWidget();
	const std::vector<std::string>& getActiveDataNameList() const;
	void updateLists(QListWidget* widget);
	QList<Graph2dHybridWindowResultSetting::XAxisMode> m_xAxisModes;
	Graph2dHybridWindowResultSetting m_setting;

	QList<Graph2dHybridWindowImportDataDataItem*> m_data;
	QList<Graph2dHybridWindowImportDataDataItem*> m_addedData;
	QList<Graph2dHybridWindowImportDataDataItem*> m_deletedData;

	Graph2dHybridWindowImportDataGroupDataItem* m_importDataGroup;

	std::vector<QWidget*> m_mainWidgets;
	std::vector<std::string> m_pointDataNameList;
	std::vector<std::string> m_oneDimDataNameList;
	std::vector<std::string> m_twoDimDataNameList;
	std::vector<std::string> m_threeDimDataNameList;

	iRICMainWindowInterface* m_mainWindow;

	Ui::Graph2dHybridWindowDataSourceDialog* ui;
};

#endif // GRAPH2DHYBRIDWINDOWDATASOURCEDIALOG_H
