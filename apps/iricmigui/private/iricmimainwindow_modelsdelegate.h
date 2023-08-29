#ifndef IRICMIMAINWINDOW_MODELSDELEGATE_H
#define IRICMIMAINWINDOW_MODELSDELEGATE_H

#include "../iricmimainwindow.h"

#include <QItemDelegate>

class iRICMIMainWindow::ModelsDelegate : public QItemDelegate
{
public:
	ModelsDelegate(QObject* parent);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // IRICMIMAINWINDOW_MODELSDELEGATE_H
