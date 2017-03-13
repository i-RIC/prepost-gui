#ifndef RIVMAKERMAINWINDOW_H
#define RIVMAKERMAINWINDOW_H

#include <QMainWindow>

#include <vector>

class QCloseEvent;

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

	void focusPreProcessorWindow();
	void focusVerticalCrossSectionWindow();
	void openCrossSectionWindow();

	void viewToggleToolBar(bool visible);
	void viewToggleStatusBar(bool visible);

	void helpMouseHint();

private:
	void closeEvent(QCloseEvent *e) override;

	void setupConnections();
	void deleteProject();

	class Impl;
	Impl* impl;

	Ui::RivmakerMainWindow *ui;
};

#ifdef _DEBUG
	#include "private/rivmakermainwindow_impl.h"
#endif // _DEBUG

#endif // RIVMAKERMAINWINDOW_H
