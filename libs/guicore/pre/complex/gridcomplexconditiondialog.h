#ifndef GRIDCOMPLEXCONDITIONDIALOG_H
#define GRIDCOMPLEXCONDITIONDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>
#include <QList>

namespace Ui
{
	class GridComplexConditionDialog;
}

class iRICMainWindowInterface;
class GridComplexConditionWidget;
//class PreProcessorRawDataComplexGroupDataItemInterface;
class PreProcessorRawDataComplexGroupDataItemInterface;
class ColorSource;

class GUICOREDLL_EXPORT GridComplexConditionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridComplexConditionDialog(PreProcessorRawDataComplexGroupDataItemInterface* item, iRICMainWindowInterface* mainWindow, QWidget* parent);
	~GridComplexConditionDialog();
	void setWidgets(QList<GridComplexConditionWidget*> widgets);
	QList<GridComplexConditionWidget*> widgets() const {return m_widgets;}

public slots:
	void accept() override;

private slots:
	void defaultChecked(bool checked);
	void selectItem(int item);
	void addItem();
	void removeItem();
	void updateCurrentName(const QString& name);

private:
	void updateList();
	QList<GridComplexConditionWidget*> m_widgets;

	PreProcessorRawDataComplexGroupDataItemInterface* m_dataItem;
	iRICMainWindowInterface* m_mainWindow;
	ColorSource* m_colorSource;
	Ui::GridComplexConditionDialog* ui;
};

#endif // GRIDCOMPLEXCONDITIONDIALOG_H
