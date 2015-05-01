#ifndef GRAPH2DHYBRIDWINDOWDRAWSETTINGDIALOG_H
#define GRAPH2DHYBRIDWINDOWDRAWSETTINGDIALOG_H

#include "../graph2dhybridwindowresultsetting.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class Graph2dHybridWindowDrawSettingDialog;
}

class Graph2dHybridWindowDrawSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindowDrawSettingDialog(QWidget* parent = nullptr);
	~Graph2dHybridWindowDrawSettingDialog();

	void setResultSettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list);
	void setCopySettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list, QList<QString> ids);
	void setImportDataSettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list, QList<QString> ids);

	const QList<Graph2dHybridWindowResultSetting::Setting>& resultSettings() const {
		return m_resultSettings;
	}
	const QList<Graph2dHybridWindowResultSetting::Setting>& copySettings() const {
		return m_copySettings;
	}
	const QList<bool>& copyDeleted() const {
		return m_copyDeleted;
	}
	const QList<Graph2dHybridWindowResultSetting::Setting>& importDataSettings() const {
		return m_importDataSettings;
	}
	const QList<bool>& importDataDeleted() const {
		return m_importDataDeleted;
	}
private slots:
	void updateResultLabel(const QString& label);
	void updateCopyLabel(const QString& label);
	void updateImportDataLabel(const QString& label);
	void setResultItem(int index);
	void setCopyItem(int index);
	void setImportDataItem(int index);
	void deleteCopyItem();
	void deleteImportDataItem();
private:
	int copyIndex(int index);
	int importDataIndex(int index);

	QList<Graph2dHybridWindowResultSetting::Setting> m_resultSettings;
	QList<Graph2dHybridWindowResultSetting::Setting> m_copySettings;
	QList<Graph2dHybridWindowResultSetting::Setting> m_importDataSettings;
	QList<QString> m_copyIds;
	QList<QString> m_importDataIds;
	QList<bool> m_copyDeleted;
	QList<bool> m_importDataDeleted;

	Ui::Graph2dHybridWindowDrawSettingDialog* ui;
};

#endif // GRAPH2DHYBRIDWINDOWDRAWSETTINGDIALOG_H
