#ifndef INSTALLERTOOL_H
#define INSTALLERTOOL_H

#include <QObject>

class iRICMainWindow;

class QString;
class QWidget;

class InstallerTool : public QObject
{
	Q_OBJECT

public:
	~InstallerTool();
	static void openMaintainanceDialog(iRICMainWindow *w);
	static void launchMaintainance();

	static QString installerFileName();

private:
	InstallerTool();
};

#endif // INSTALLERTOOL_H
