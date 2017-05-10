#ifndef IRICMAINWINDOWABOUTDIALOG_H
#define IRICMAINWINDOWABOUTDIALOG_H

#include <QDialog>

class iRICMetaData;

namespace Ui
{
	class iRICMainWindowAboutDialog;
}

/// This class handles the about dialog of iRIC.
class iRICMainWindowAboutDialog : public QDialog
{
	Q_OBJECT

public:
	iRICMainWindowAboutDialog(QWidget* parent = nullptr);
	~iRICMainWindowAboutDialog();

	void init(const iRICMetaData& data);

private:
	Ui::iRICMainWindowAboutDialog* ui;
};

#endif // IRICMAINWINDOWABOUTDIALOG_H
