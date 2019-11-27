#ifndef PARTICLEARBITRARYTIMEEDITDIALOG_H
#define PARTICLEARBITRARYTIMEEDITDIALOG_H

#include "../postbase_global.h"

#include <QDialog>

#include <vector>

namespace Ui {
class ParticleArbitraryTimeEditDialog;
}

class ProjectMainFile;

class POSTBASEDLL_EXPORT ParticleArbitraryTimeEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ParticleArbitraryTimeEditDialog(QWidget *parent = 0);
	~ParticleArbitraryTimeEditDialog();

	void setMainFile(ProjectMainFile* file);
	void setTimeSteps(const std::vector<int>& steps);

	std::vector<int> timeSteps() const;

private slots:
	void add();
	void removeSelected();

private:
	void updateList();

	std::vector<int> m_timeSteps;
	ProjectMainFile* m_mainFile;
	Ui::ParticleArbitraryTimeEditDialog *ui;
};

#endif // PARTICLEARBITRARYTIMEEDITDIALOG_H
