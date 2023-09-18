#include "backgroundcolorediti.h"
#include "private/backgroundcolorediti_setbackgroundcolorcommand.h"

#include <misc/iricundostack.h>

#include <QColorDialog>
#include <QObject>

BackgroundColorEditI::~BackgroundColorEditI()
{}

void BackgroundColorEditI::editBackgroundColor(QWidget* w)
{
	QColor c = backgroundColor();
	QColor newc = QColorDialog::getColor(c, w, QObject::tr("Background Color"));
	if (! newc.isValid()) {return;}

	iRICUndoStack::instance().push(new SetBackgroundColorCommand(newc, this));
}
