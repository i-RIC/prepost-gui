#ifndef GRIDCREATINGCONDITIONPOISSONGRIDGENERATEDIALOG_H
#define GRIDCREATINGCONDITIONPOISSONGRIDGENERATEDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionPoissonGridGenerateDialog;
}

class GridCreatingConditionPoissonGridGenerateDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionPoissonGridGenerateDialog(QWidget *parent = 0);
	~GridCreatingConditionPoissonGridGenerateDialog();

	void setReadOnly(bool readOnly);

	int iDiv() const;
	void setIDiv(int div);

	int jDiv() const;
	void setJDiv(int div);

	int maxIterations() const;
	void setMaxIterations(int iters);

	void setILength(double len);
	void setJLength(double len);

private slots:
	void updateDI();
	void updateDJ();

private:
	Ui::GridCreatingConditionPoissonGridGenerateDialog *ui;

	double m_iLength;
	double m_jLength;
};

#endif // GRIDCREATINGCONDITIONPOISSONGRIDGENERATEDIALOG_H
