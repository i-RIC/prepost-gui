#include "pythonutil.h"
#include "iricrootpath.h"

#include <QDir>

QString PythonUtil::defaultPath()
{
	QDir dir(iRICRootPath::get());
	dir.cdUp(); // gui
	dir.cdUp(); // iRIC
	return dir.absoluteFilePath("Miniconda3/envs/iric/python.exe");
}
