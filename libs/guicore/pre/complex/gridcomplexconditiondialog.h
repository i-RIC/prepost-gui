#ifndef GRIDCOMPLEXCONDITIONDIALOG_H
#define GRIDCOMPLEXCONDITIONDIALOG_H

#include "../../guicore_global.h"
#include <QDialog>

#include <vector>

namespace Ui
{
	class GridComplexConditionDialog;
}

class iRICMainWindowI;
class GridComplexConditionWidget;
class PreProcessorGeoDataComplexGroupDataItemI;
class ColorSource;

class GUICOREDLL_EXPORT GridComplexConditionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridComplexConditionDialog(PreProcessorGeoDataComplexGroupDataItemI* item, QWidget* parent);
	~GridComplexConditionDialog();

	std::vector<GridComplexConditionWidget*> widgets() const;
	void setWidgets(std::vector<GridComplexConditionWidget*> widgets);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void defaultChecked(bool checked);
	void selectItem(int item);
	void addItem();
	void removeItem();
	void updateCurrentName(const QString& name);

private:
	void updateList();

	std::vector<GridComplexConditionWidget*> m_widgets;

	PreProcessorGeoDataComplexGroupDataItemI* m_dataItem;
	ColorSource* m_colorSource;
	Ui::GridComplexConditionDialog* ui;
};

#endif // GRIDCOMPLEXCONDITIONDIALOG_H
