#include "backgroundcoloreditinterface.h"
#include "private/backgroundcoloreditinterface_setbackgroundcolorcommand.h"

#include <misc/iricundostack.h>

#include <QColorDialog>
#include <QObject>

BackgroundColorEditInterface::~BackgroundColorEditInterface()
{}

void BackgroundColorEditInterface::editBackgroundColor(QWidget* w)
{
	QColor c = backgroundColor();
	QColor newc = QColorDialog::getColor(c, w, QObject::tr("Background Color"));
	if (! newc.isValid()) {return;}

	iRICUndoStack::instance().push(new SetBackgroundColorCommand(newc, this));
}
