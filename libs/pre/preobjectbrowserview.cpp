#include "preobjectbrowserview.h"
#include <QMouseEvent>
#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QUndoCommand>

#include <misc/iricundostack.h>

PreObjectBrowserView::PreObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::EditKeyPressed);
	setEditTriggers(triggers);
}
