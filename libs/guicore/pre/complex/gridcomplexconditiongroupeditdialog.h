#ifndef GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H
#define GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H

#include "../../guicore_global.h"

#include <QDialog>

class InputConditionContainerSet;
class GridComplexConditionGroup;

namespace Ui {
class GridComplexConditionGroupEditDialog;
}

class QAbstractButton;

class GUICOREDLL_EXPORT GridComplexConditionGroupEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridComplexConditionGroupEditDialog(QWidget *parent = 0);
	~GridComplexConditionGroupEditDialog();

	void setGroups(const std::vector<GridComplexConditionGroup*>& groups);
	void setCurrentIndex(int index);

public slots:
	void handleIndexChange(int index);
	void handleModified();

	void accept() override;
	void reject() override;

private:
	void setTargetGroup(GridComplexConditionGroup* group);
	void updateTargetComboBox();
	void clearBackups();

	std::vector<GridComplexConditionGroup*> m_groups;
	std::vector<InputConditionContainerSet*> m_backups;

	int m_currentIndex;
	bool m_modified;
	Ui::GridComplexConditionGroupEditDialog *ui;
};

#endif // GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H
