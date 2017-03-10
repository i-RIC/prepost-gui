#ifndef RIVMAKERMAINWINDOW_H
#define RIVMAKERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class RivmakerMainWindow;
}

class RivmakerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit RivmakerMainWindow(QWidget *parent = 0);
	~RivmakerMainWindow();

private:
	Ui::RivmakerMainWindow *ui;
};

#endif // RIVMAKERMAINWINDOW_H
