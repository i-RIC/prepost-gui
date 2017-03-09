#ifndef GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H
#define GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H

#include "../../guicore_global.h"

#include <QDialog>

class InputConditionContainerSet;
class GridComplexConditionGroup;

namespace Ui {
class GridComplexConditionGroupEditDialog;
}

class GUICOREDLL_EXPORT GridComplexConditionGroupEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridComplexConditionGroupEditDialog(QWidget *parent = 0);
	~GridComplexConditionGroupEditDialog();

	void setGroup(GridComplexConditionGroup* group);

public slots:
	void reject() override;

private:
	GridComplexConditionGroup* m_group;
	InputConditionContainerSet* m_backup;
	Ui::GridComplexConditionGroupEditDialog *ui;
};

#endif // GRIDCOMPLEXCONDITIONGROUPEDITDIALOG_H
