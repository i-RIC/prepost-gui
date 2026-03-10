#ifndef PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_CLIPDIALOG_H
#define PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_CLIPDIALOG_H

#include "../preprocessorstructured2dgridshapedataitem.h"

#include <QDialog>

namespace Ui {
class PreProcessorStructured2dGridShapeDataItem_ClipDialog;
}

class PreProcessorStructured2dGridShapeDataItem::ClipDialog : public QDialog
{
	Q_OBJECT

public:
	ClipDialog(PreProcessorStructured2dGridShapeDataItem* dataItem, QWidget *parent);
	~ClipDialog();

private:
	PreProcessorStructured2dGridShapeDataItem* m_dataItem;
	Ui::PreProcessorStructured2dGridShapeDataItem_ClipDialog *ui;
};

#endif // PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_CLIPDIALOG_H
