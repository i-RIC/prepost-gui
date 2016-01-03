#ifndef COLORTRANSFERFUNCTIONEDITWIDGET_H
#define COLORTRANSFERFUNCTIONEDITWIDGET_H

#include "../guicore_global.h"
#include "scalarstocolorseditwidget.h"
#include <QColor>
#include <QList>

class QTableWidgetItem;

namespace Ui
{
	class ColorTransferFunctionEditWidget;
}

class LookupTableContainer;

class GUICOREDLL_EXPORT ColorTransferFunctionEditWidget : public ScalarsToColorsEditWidget
{
	Q_OBJECT

private:
	static const int ATTRWIDTH;
	static const int COLORWIDTH;

public:
	explicit ColorTransferFunctionEditWidget(QWidget* parent = nullptr);
	virtual ~ColorTransferFunctionEditWidget();

	void save() override;

private slots:
	void handleItemEdit(QTableWidgetItem* item);
	void handleItemClick(QTableWidgetItem* item);

private:
	void setupWidget() override;

	Ui::ColorTransferFunctionEditWidget* ui;
	QList<QColor> m_colors;
};

#endif // COLORTRANSFERFUNCTIONEDITWIDGET_H
