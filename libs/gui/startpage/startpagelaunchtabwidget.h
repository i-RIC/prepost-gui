#ifndef STARTPAGELAUNCHTABWIDGET_H
#define STARTPAGELAUNCHTABWIDGET_H

#include <QWidget>

namespace Ui
{
	class StartPageLaunchTabWidget;
}

class SolverDefinitionList;
class SolverDefinitionAbstract;

class StartPageLaunchTabWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartPageLaunchTabWidget(QWidget* parent = nullptr);
	~StartPageLaunchTabWidget();
	void setSolverList(SolverDefinitionList* solverList);

signals:
	void newProjectButtonClicked();
	void openProjectButtonClicked();
	void projectFileSelected(const QString& filename);
	void solverSelected(SolverDefinitionAbstract* solverDef);

private slots:
	void openProject();

private:
	Ui::StartPageLaunchTabWidget* ui;
};

#endif // STARTPAGELAUNCHTABWIDGET_H
