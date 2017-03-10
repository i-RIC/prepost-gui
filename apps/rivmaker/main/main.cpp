#include "rivmakermainwindow.h"

#include <vtkAutoInit.h>

#include <QApplication>
#include <QSettings>

VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingFreeTypeOpenGL);

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
/*
	// install translators. it borrows iRIC GUI language setting.
	QSettings settings("iRIC Organization", "iRIC GUI");
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();

	QString exeFolder = QApplication::applicationDirPath();
	iRICRootPath::set(exeFolder);

	QString langFolder = QDir(exeFolder).absoluteFilePath("languages");

	QStringList nameFilters;
	nameFilters.append(QString("*_%1.qm").arg(locale));
	QDir langDir(langFolder);
	foreach (QString fileName, langDir.entryList(nameFilters, QDir::Files)) {
		QFileInfo finfo(langDir.filePath(fileName));
		QTranslator* translator = new QTranslator(&a);
		translator->load(finfo.baseName(), finfo.absolutePath());
		a.installTranslator(translator);
	}
*/

	RivmakerMainWindow w;
	w.show();
	return a.exec();
}
