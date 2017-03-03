#ifndef GRIDCOMPLEXCONDITIONDIALOG_H
#define GRIDCOMPLEXCONDITIONDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>

#include <vector>

namespace Ui
{
	class GridComplexConditionDialog;
}

class iRICMainWindowInterface;
class GridComplexConditionWidget;
class PreProcessorGeoDataComplexGroupDataItemInterface;
class ColorSource;

class GUICOREDLL_EXPORT GridComplexConditionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridComplexConditionDialog(PreProcessorGeoDataComplexGroupDataItemInterface* item, iRICMainWindowInterface* mainWindow, QWidget* parent);
	~GridComplexConditionDialog();

	std::vector<GridComplexConditionWidget*> widgets() const;
	void setWidgets(std::vector<GridComplexConditionWidget*> widgets);

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

	std::vector<GridComplexConditionWidget*> m_widgets;

	PreProcessorGeoDataComplexGroupDataItemInterface* m_dataItem;
	iRICMainWindowInterface* m_mainWindow;
	ColorSource* m_colorSource;
	Ui::GridComplexConditionDialog* ui;
};

#endif // GRIDCOMPLEXCONDITIONDIALOG_H
