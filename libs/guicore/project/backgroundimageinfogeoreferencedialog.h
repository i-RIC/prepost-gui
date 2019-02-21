#ifndef BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_H
#define BACKGROUNDIMAGEINFOGEOREFERENCEDIALOG_H

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
	explicit BackgroundImageInfoGeoreferenceDialog(BackgroundImageInfo* info, QWidget* parent = 0);
	~BackgroundImageInfoGeoreferenceDialog();

	void clearSelection();
	void setSelection(const std::unordered_set<std::vector<GcpTableRow>::size_type>& indices);

	std::vector<GcpTableRow>* gcpTable();
	AddibleGcpTableModel* gcpTableModel();

	double leftBottomX() const;
	double leftBottomY() const;
	double scale() const;
	double angle() const;

	double origLeftBottomX() const;
	double origLeftBottomY() const;
	double origScale() const;
	double origAngle() const;

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

	BackgroundImageInfo* m_info;

	double m_leftbottomX;
	double m_leftbottomY;
	double m_scale;
	double m_angle;

	double m_origLeftbottomX;
	double m_origLeftbottomY;
	double m_origScale;
	double m_origAngle;

	std::unique_ptr<AddibleGcpTableModel> m_gcpTableModel;
	Ui::BackgroundImageInfoGeoreferenceDialog *ui;

	class StyledItemDelegate;
};

#endif // BACKGROUNDIMAGEITEMGEOREFERENCEDIALOG_H
