#include "rivmakermainwindow.h"

#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	// install translators. it borrows iRIC GUI language setting.
	QSettings settings("iRIC Organization", "iRIC GUI 3");
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

	RivmakerMainWindow w;
	w.show();
	return a.exec();
}
