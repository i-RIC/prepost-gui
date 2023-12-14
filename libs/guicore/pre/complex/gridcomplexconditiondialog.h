#ifndef GRIDCOMPLEXCONDITIONDIALOG_H
#define GRIDCOMPLEXCONDITIONDIALOG_H

#include "../../guicore_global.h"

#include <QDialog>

#include <memory>
#include <vector>

namespace Ui
{
	class GridComplexConditionDialog;
}

namespace iRICLib {
	class H5CgnsGridComplexConditionGroup;
} // iRICLib

class GridComplexConditionGroup;
class GridComplexConditionWidget;
class ColorSource;
class SolverDefinition;

class GUICOREDLL_EXPORT GridComplexConditionDialog : public QDialog
{
	Q_OBJECT

public:
	GridComplexConditionDialog(SolverDefinition* def, const QDomElement& elem, QWidget* parent);
	~GridComplexConditionDialog();

	const std::vector<std::shared_ptr<GridComplexConditionGroup> > groups() const;

	void createDefaultGroup();
	void setupGroups(int count);
	void clear();
	int loadFromCgnsFile(iRICLib::H5CgnsGridComplexConditionGroup* group);
	int saveToCgnsFile(iRICLib::H5CgnsGridComplexConditionGroup* group);
	bool importFromCsvFile(const QString& filename, QWidget* w);
	bool exportToCsvFile(const QString& filename);

	void setCalculationConditionMode(bool mode);

public slots:
	int exec() override;
	void accept() override;
	void reject() override;

private slots:
	void defaultChecked(bool checked);
	void selectItem(int item);
	void addItem();
	void removeItem();
	void updateCurrentName(const QString& name);
	void switchView();
	void importCsvFile();
	void exportCsvFile();

private:
	void updateList();
	void updateTable();
	void updateSwitchButtonText();

	std::vector<GridComplexConditionWidget*> m_widgets;
	std::vector<std::shared_ptr<GridComplexConditionGroup> > m_groups;
	std::vector<std::shared_ptr<GridComplexConditionGroup> > m_backupGroups;

	SolverDefinition* m_solverDefinition;
	QDomElement m_element;
	ColorSource* m_colorSource;

	QColor m_undefinedColor;
	bool m_calculationConditionMode;
	Ui::GridComplexConditionDialog* ui;
};

#endif // GRIDCOMPLEXCONDITIONDIALOG_H
