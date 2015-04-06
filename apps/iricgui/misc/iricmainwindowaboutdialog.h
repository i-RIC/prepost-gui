#ifndef IRICMAINWINDOWABOUTDIALOG_H
#define IRICMAINWINDOWABOUTDIALOG_H

#include <QDialog>
class QDomNode;

namespace Ui {
	class iRICMainWindowAboutDialog;
}

/// This class handles the about dialog of iRIC.
class iRICMainWindowAboutDialog : public QDialog {
	Q_OBJECT
public:
	/// Constructor
	iRICMainWindowAboutDialog(QWidget *parent = 0);
	/// Destructor
	~iRICMainWindowAboutDialog();
	/// Initialization of dialog
	void init(const QDomNode& data);
private:
	Ui::iRICMainWindowAboutDialog *ui;
};

#endif // IRICMAINWINDOWABOUTDIALOG_H
