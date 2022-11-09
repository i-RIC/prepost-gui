#ifndef BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_H
#define BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_H

#include "private/backgroundimageinfo_setting.h"

#include <QDialog>

#include <memory>
#include <unordered_set>

class QItemSelection;
class QCloseEvent;

namespace Ui {
	class BackgroundImageInfoGeoreferenceDialog;
}

class BackgroundImageInfo;
class AddibleGcpTableModel;
class GcpTableRow;

class BackgroundImageInfoGeoreferenceDialog : public QDialog
{
	Q_OBJECT

public:
	BackgroundImageInfoGeoreferenceDialog(BackgroundImageInfo* info, QWidget* parent = 0);
	~BackgroundImageInfoGeoreferenceDialog();

	void clearSelection();
	void setSelection(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);

	std::vector<GcpTableRow>* gcpTable();
	AddibleGcpTableModel* gcpTableModel();

	BackgroundImageInfo::Setting setting() const;

public slots:
	void accept() override;
	void reject() override;
	void apply();

signals:
	void selectionChanged(const std::unordered_set<std::vector<GcpTableRow>::size_type>& rowIndices);

private slots:
	void handleSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void handleHideButtonClicked();
	void setHideButtonText();

private:
	void calculate();

	BackgroundImageInfo::Setting m_setting;
	BackgroundImageInfo::Setting m_originalSetting;

	BackgroundImageInfo* m_info;

	std::unique_ptr<AddibleGcpTableModel> m_gcpTableModel;
	Ui::BackgroundImageInfoGeoreferenceDialog *ui;

	class StyledItemDelegate;
};

#endif // BACKGROUNDIMAGEITEMGEOREFERENCEDIALOG_H
