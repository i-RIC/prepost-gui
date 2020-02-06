#ifndef COORDINATESEDITDIALOG_H
#define COORDINATESEDITDIALOG_H

#include <QDialog>

#include <vector>

namespace Ui {
class CoordinatesEditDialog;
}

class QAbstractButton;
class QPointF;
class QStandardItemModel;

using accept_callback = void (*)(const std::vector<QPointF>& coords);
using reject_callback = void (*)();
using apply_callback = void (*)(const std::vector<QPointF>& coords);

class CoordinatesEditDialog : public QDialog
{
	Q_OBJECT

private:
	static const int DEFAULT_ROW_HEIGHT = 20;

public:
	explicit CoordinatesEditDialog(QWidget *parent = 0);
	~CoordinatesEditDialog();

	void setCoordinates(const std::vector<QPointF> coords);

	void setAcceptCallback(accept_callback callback);
	void setRejectCallback(reject_callback callback);
	void setApplyCallback(apply_callback callback);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	std::vector<QPointF> getCoordinates() const;

	Ui::CoordinatesEditDialog *ui;

	accept_callback m_acceptCallback;
	reject_callback m_rejectCallback;
	apply_callback m_applyCallback;

	QStandardItemModel* m_model;
};

#endif // COORDINATESEDITDIALOG_H
