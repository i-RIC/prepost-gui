#ifndef RIVMAKERMAINWINDOW_H
#define RIVMAKERMAINWINDOW_H

#include "window/verticalcrosssection/verticalcrosssectionwindow.h"

#include <QMainWindow>

#include <vector>

class CrossSectionWindow;
class Project;

namespace Ui {
class RivmakerMainWindow;
}

class RivmakerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit RivmakerMainWindow(QWidget *parent = 0);
	~RivmakerMainWindow();

private slots:
	void newProject();

	void focusVerticalCrossSectionWindow();
	void openCrossSectionWindow();

	void viewToggleToolBar(bool visible);
	void viewToggleStatusBar(bool visible);

	void helpMouseHint();

private:
	void setupConnections();
	void deleteProject();

	VerticalCrossSectionWindow m_verticalCrossSectionWindow;
	std::vector<CrossSectionWindow*> m_crossSectionWindows;

	Project* m_project;

	Ui::RivmakerMainWindow *ui;
};

#endif // RIVMAKERMAINWINDOW_H
