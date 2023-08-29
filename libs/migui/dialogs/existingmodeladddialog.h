#ifndef EXISTINGMODELADDDIALOG_H
#define EXISTINGMODELADDDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

class iRICMIProject;
class Model;
class SolverDefinitionList;

#include <vector>

namespace Ui {
class ExistingModelAddDialog;
}

class ExistingModelAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ExistingModelAddDialog(QWidget *parent = nullptr);
	~ExistingModelAddDialog();

	std::vector<Model*> models() const;
	void setModels(const std::vector<Model*>& models);

	static std::vector<Model*> buildModels(iRICMIProject* project, SolverDefinitionList* solvers);

public slots:
	void accept();
	void reject();

private:
	std::vector<Model*> m_models;
	QStandardItemModel m_model;

	Ui::ExistingModelAddDialog *ui;
};

#endif // EXISTINGMODELADDDIALOG_H
