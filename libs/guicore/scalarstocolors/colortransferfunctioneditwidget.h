#ifndef COLORTRANSFERFUNCTIONEDITWIDGET_H
#define COLORTRANSFERFUNCTIONEDITWIDGET_H

#include "../guicore_global.h"
#include "scalarstocolorseditwidget.h"
#include <QColor>
#include <QList>

class QTableWidgetItem;

namespace Ui {
    class ColorTransferFunctionEditWidget;
}

class LookupTableContainer;

class GUICOREDLL_EXPORT ColorTransferFunctionEditWidget : public ScalarsToColorsEditWidget
{
	Q_OBJECT

public:
	static const int ATTRWIDTH = 170;
	static const int COLORWIDTH = 50;
	explicit ColorTransferFunctionEditWidget(QWidget *parent = nullptr);
	virtual ~ColorTransferFunctionEditWidget();
	void save();

protected:
	void setupWidget();

private slots:
	void handleItemEdit(QTableWidgetItem * item);
	void handleItemClick(QTableWidgetItem * item);

private:
	Ui::ColorTransferFunctionEditWidget *ui;
	QList<QColor> m_colors;
};

#endif // COLORTRANSFERFUNCTIONEDITWIDGET_H
