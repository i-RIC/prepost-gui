#include <migui/main/iricmimainwindow.h>
#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("iRIC Organization");
	QCoreApplication::setOrganizationDomain("i-ric.org");
	QCoreApplication::setApplicationName("iRIC GUI 4");

	// install translators.
	QSettings settings;
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

	// Qt dictionary
	QString transFolder = QDir(exeFolder).absoluteFilePath("translations");
	auto locale_short = locale;
	if (locale != "zh_TW") {
		locale_short = locale.left(2);
	}
	QTranslator* translator = new QTranslator(&a);
	translator->load(QString("qt_%1.qm").arg(locale_short), transFolder);
	a.installTranslator(translator);

	iRICMIMainWindow w;
	w.show();
	return a.exec();
}
