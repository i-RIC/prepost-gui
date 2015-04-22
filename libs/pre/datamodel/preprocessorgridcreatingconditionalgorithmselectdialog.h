#ifndef PREPROCESSORGRIDCREATINGCONDITIONALGORITHMSELECTDIALOG_H
#define PREPROCESSORGRIDCREATINGCONDITIONALGORITHMSELECTDIALOG_H

#include <QDialog>
#include <QList>

class GridCreatingConditionCreator;

namespace Ui {
	class PreProcessorGridCreatingConditionAlgorithmSelectDialog;
}

class PreProcessorGridCreatingConditionAlgorithmSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreProcessorGridCreatingConditionAlgorithmSelectDialog(QWidget *parent = nullptr);
	~PreProcessorGridCreatingConditionAlgorithmSelectDialog();
	void setCreators(const QList<GridCreatingConditionCreator*>& creators);
	void setCurrent(GridCreatingConditionCreator* creator);
	GridCreatingConditionCreator* selectedCreator();
private slots:
	void handleCurrentChange(int index);
private:
	Ui::PreProcessorGridCreatingConditionAlgorithmSelectDialog *ui;
	QList<GridCreatingConditionCreator*> m_creators;
};

#endif // PREPROCESSORGRIDCREATINGCONDITIONALGORITHMSELECTDIALOG_H
