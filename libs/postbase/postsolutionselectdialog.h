#ifndef POSTSOLUTIONSELECTDIALOG_H
#define POSTSOLUTIONSELECTDIALOG_H

#include <QDialog>
#include <QString>

#include "postbase_global.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace Ui {
class PostSolutionSelectDialog;
}

class POSTBASEDLL_EXPORT PostSolutionSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostSolutionSelectDialog(QWidget *parent = nullptr);
	~PostSolutionSelectDialog();

	void setSolutions(const std::unordered_map<std::string, QString>& solutions);
	std::string selectedSolution() const;

private:
	std::vector<std::string> m_solutions;

	Ui::PostSolutionSelectDialog *ui;
};

#endif // POSTSOLUTIONSELECTDIALOG_H
