#ifndef COLORTRANSFERFUNCTIONEDITDIALOG_H
#define COLORTRANSFERFUNCTIONEDITDIALOG_H

#include "scalarstocolorseditdialog.h"

class ColorTransferFunctionEditDialog : public ScalarsToColorsEditDialog
{

public:
	ColorTransferFunctionEditDialog(QWidget* parent)
		: ScalarsToColorsEditDialog(parent)
	{}

protected:
	void setupDialog();
};

#endif // COLORTRANSFERFUNCTIONEDITDIALOG_H
