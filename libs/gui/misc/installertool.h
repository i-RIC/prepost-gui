#ifndef INSTALLERTOOL_H
#define INSTALLERTOOL_H

class QString;

class InstallerTool
{

public:
	~InstallerTool();
	static void openMaintainanceDialog();
	static QString installerFileName();

private:
	InstallerTool();
};

#endif // INSTALLERTOOL_H
