#ifndef PREPROCESSORWINDOW_H
#define PREPROCESSORWINDOW_H

#include <QMainWindow>

class PreProcessorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit PreProcessorWindow(QWidget *parent = 0);
	~PreProcessorWindow();

private:
	void closeEvent(QCloseEvent *e);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/preprocessorwindow_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORWINDOW_H
