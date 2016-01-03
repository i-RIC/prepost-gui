#ifndef SCALARSTOCOLORSEDITDIALOG_H
#define SCALARSTOCOLORSEDITDIALOG_H

#include "../guicore_global.h"
#include <QDialog>

class ScalarsToColorsContainer;
class ScalarsToColorsEditWidget;

namespace Ui
{
	class ScalarsToColorsEditDialog;
}

class GUICOREDLL_EXPORT ScalarsToColorsEditDialog : public QDialog
{

public:
	ScalarsToColorsEditDialog(QWidget* parent = nullptr);
	virtual ~ScalarsToColorsEditDialog();

	ScalarsToColorsEditWidget* widget() const;
	void setWidget(ScalarsToColorsEditWidget* w);

	void setContainer(ScalarsToColorsContainer* container);

	void accept() override;

private:
	ScalarsToColorsEditWidget* m_widget;
	Ui::ScalarsToColorsEditDialog* ui;
};

#endif // SCALARSTOCOLORSEDITDIALOG_H
