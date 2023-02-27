#include "postsolutionselectdialog.h"
#include "ui_postsolutionselectdialog.h"

#include <map>

PostSolutionSelectDialog::PostSolutionSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostSolutionSelectDialog)
{
	ui->setupUi(this);
}

PostSolutionSelectDialog::~PostSolutionSelectDialog()
{
	delete ui;
}

void PostSolutionSelectDialog::setSolutions(const std::unordered_map<std::string, QString>& solutions)
{
	std::map<QString, std::string> reverseMap;

	for (const auto& pair : solutions) {
		reverseMap.insert({pair.second, pair.first});
	}

	for (const auto& pair : reverseMap) {
		ui->solutionComboBox->addItem(pair.first);
		m_solutions.push_back(pair.second);
	}
}

std::string PostSolutionSelectDialog::selectedSolution() const
{
	return m_solutions.at(ui->solutionComboBox->currentIndex());
}
