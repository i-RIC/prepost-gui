#ifndef COORDINATESEDITDIALOG_H
#define COORDINATESEDITDIALOG_H

#include <QDialog>

class QAbstractButton;
class CoordinatesI;

namespace Ui
{
	class CoordinatesEditDialog;
}

class CoordinatesEditDialog : public QDialog
{
	Q_OBJECT

public:
	CoordinatesEditDialog(QWidget* parent = nullptr);
	~CoordinatesEditDialog();

	void setOffset(const QPointF& offset);
	void setTarget(CoordinatesI* target);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void setupData();
	void apply();
	std::vector<QPointF> getCoords() const;

	Ui::CoordinatesEditDialog* ui;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/coordinateseditdialog_impl.h"
#endif // _DEBUG

#endif // COORDINATESEDITDIALOG_H
