#ifndef IRICMIMAINWINDOW_MODELSTABLEDELEGATE_H
#define IRICMIMAINWINDOW_MODELSTABLEDELEGATE_H

#include "../iricmimainwindow.h"

#include <QItemDelegate>

class iRICMIMainWindow::ModelsTableDelegate : public QItemDelegate
{
public:
	ModelsTableDelegate(QObject* parent);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // IRICMIMAINWINDOW_MODELSTABLEDELEGATE_H
