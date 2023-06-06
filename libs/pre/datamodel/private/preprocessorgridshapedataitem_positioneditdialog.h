#ifndef PREPROCESSORGRIDSHAPEDATAITEM_POSITIONEDITDIALOG_H
#define PREPROCESSORGRIDSHAPEDATAITEM_POSITIONEDITDIALOG_H

#include "../preprocessorgridshapedataitem.h"

#include <QDialog>

namespace Ui {
class PreProcessorGridShapeDataItem_PositionEditDialog;
}

class QPointF;

class PreProcessorGridShapeDataItem::PositionEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PositionEditDialog(QWidget *parent);
	~PositionEditDialog();

	QPointF position() const;
	void setPosition(const QPointF& position);
	void setPosition(double x, double y);

	void setIndex(int index);
	void setIJ(int i, int j);

private:
	Ui::PreProcessorGridShapeDataItem_PositionEditDialog *ui;
};

#endif // PREPROCESSORGRIDSHAPEDATAITEM_POSITIONEDITDIALOG_H
