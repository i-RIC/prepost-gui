#if SKIP
#ifndef GRAPH2DVERIFICATIONWINDOWDRAWSETTINGDIALOG_H
#define GRAPH2DVERIFICATIONWINDOWDRAWSETTINGDIALOG_H

#include "../graph2dverificationwindowresultsetting.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class Graph2dVerificationWindowDrawSettingDialog;
}

class Graph2dVerificationWindowDrawSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindowDrawSettingDialog(QWidget* parent = nullptr);
	~Graph2dVerificationWindowDrawSettingDialog();

	void setResultSettings(const QList<Graph2dVerificationWindowResultSetting::Setting>& list);
	void setCopySettings(const QList<Graph2dVerificationWindowResultSetting::Setting>& list, QList<QString> ids);
	void setImportDataSettings(const QList<Graph2dVerificationWindowResultSetting::Setting>& list, QList<QString> ids);

	const QList<Graph2dVerificationWindowResultSetting::Setting>& resultSettings() const {
		return m_resultSettings;
	}
	const QList<Graph2dVerificationWindowResultSetting::Setting>& copySettings() const {
		return m_copySettings;
	}
	const QList<bool>& copyDeleted() const {
		return m_copyDeleted;
	}
	const QList<Graph2dVerificationWindowResultSetting::Setting>& importDataSettings() const {
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

	QList<Graph2dVerificationWindowResultSetting::Setting> m_resultSettings;
	QList<Graph2dVerificationWindowResultSetting::Setting> m_copySettings;
	QList<Graph2dVerificationWindowResultSetting::Setting> m_importDataSettings;
	QList<QString> m_copyIds;
	QList<QString> m_importDataIds;
	QList<bool> m_copyDeleted;
	QList<bool> m_importDataDeleted;

	Ui::Graph2dVerificationWindowDrawSettingDialog* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWDRAWSETTINGDIALOG_H
#endif
