#ifndef SCALARSTOCOLORSEDITDIALOG_H
#define SCALARSTOCOLORSEDITDIALOG_H

#include "../guicore_global.h"
#include <QDialog>

class ScalarsToColorsContainer;
class ScalarsToColorsEditWidget;

namespace Ui {
	class ScalarsToColorsEditDialog;
}

class GUICOREDLL_EXPORT ScalarsToColorsEditDialog : public QDialog
{
public:
	ScalarsToColorsEditDialog(QWidget *parent = 0);
	void setWidget(ScalarsToColorsEditWidget* w);
	ScalarsToColorsEditWidget* widget(){return m_widget;}
	void setContainer(ScalarsToColorsContainer* container);
	void accept();
protected:
	ScalarsToColorsEditWidget* m_widget;
	Ui::ScalarsToColorsEditDialog *ui;
};

#endif // SCALARSTOCOLORSEDITDIALOG_H
