#ifndef PREPROCESSORGRIDSHAPEDATAITEM_POSITIONDELTADIALOG_H
#define PREPROCESSORGRIDSHAPEDATAITEM_POSITIONDELTADIALOG_H

#include "../preprocessorgridshapedataitem.h"

#include <QDialog>

namespace Ui {
class PreProcessorGridShapeDataItem_PositionDeltaDialog;
}

class PreProcessorGridShapeDataItem::PositionDeltaDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PositionDeltaDialog(QWidget *parent);
	~PositionDeltaDialog();

	QPointF delta() const;

private:
	Ui::PreProcessorGridShapeDataItem_PositionDeltaDialog *ui;
};

#endif // PREPROCESSORGRIDSHAPEDATAITEM_POSITIONDELTADIALOG_H
