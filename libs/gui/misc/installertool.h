#ifndef INSTALLERTOOL_H
#define INSTALLERTOOL_H

#include <QObject>

class QString;
class QWidget;

class InstallerTool : public QObject
{
	Q_OBJECT

public:
	~InstallerTool();
	static void openMaintainanceDialog(QWidget* w);
	static QString installerFileName();

private:
	InstallerTool();
};

#endif // INSTALLERTOOL_H
