#include "preobjectbrowserview.h"

#include <misc/iricundostack.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QMouseEvent>
#include <QUndoCommand>

PreObjectBrowserView::PreObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::EditKeyPressed);
	setEditTriggers(triggers);
}
