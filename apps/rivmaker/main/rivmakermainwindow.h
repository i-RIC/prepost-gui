#ifndef RIVMAKERMAINWINDOW_H
#define RIVMAKERMAINWINDOW_H

#include <QMainWindow>

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

	void viewToggleToolBar(bool visible);
	void viewToggleStatusBar(bool visible);

	void helpMouseHint();

private:
	void setupConnections();
	void deleteProject();

	Project* m_project;

	Ui::RivmakerMainWindow *ui;
};

#endif // RIVMAKERMAINWINDOW_H
