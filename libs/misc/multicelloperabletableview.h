#ifndef MULTICELLOPERABLETABLEVIEW_H
#define MULTICELLOPERABLETABLEVIEW_H

#include "misc_global.h"

#include <QTableView>

class MISCDLL_EXPORT MultiCellOperableTableView : public QTableView
{
	Q_OBJECT
public:
	MultiCellOperableTableView(QWidget* parent = Q_NULLPTR);

protected:
	void keyPressEvent(QKeyEvent* event) override;
};

#endif // MULTICELLOPERABLETABLEVIEW_H
