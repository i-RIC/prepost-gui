#ifndef CONNECTIONADDDIALOG_H
#define CONNECTIONADDDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionAddDialog;
}

class ConnectionInput;
class ConnectionOutput;
class iRICMIProject;
class Model;

class ConnectionAddDialog : public QDialog
{
	Q_OBJECT

public:
	ConnectionAddDialog(iRICMIProject* project, QWidget *parent);
	~ConnectionAddDialog();

	ConnectionOutput* output() const;
	ConnectionInput* input() const;

private slots:
	void handleOutputModelChange(int index);
	void handleOutputValueChange(int index);
	void handleInputModelChange(int index);

private:
	void setupOutputModels();

	std::vector<Model*> m_outputModels;
	Model* m_outputModel;

	std::vector<Model*> m_inputModels;
	std::vector<ConnectionInput*> m_inputValues;

	iRICMIProject* m_project;

	Ui::ConnectionAddDialog *ui;
};

#endif // CONNECTIONADDDIALOG_H
