#include "rivmakermainwindow.h"
#include "../misc/geometryutil.h"


#include <QApplication>
#include <QSettings>

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
	/*
	QPointF isec;
	double r, s;
	bool ok;

	ok = GeometryUtil::intersectionPoint(QPointF(-1, 0), QPointF(1, 0), QPointF(0, -1), QPointF(0, 1), &isec, &r, &s);
	ok = GeometryUtil::intersectionPoint(QPointF(-3, 0), QPointF(1, 0), QPointF(0, -1), QPointF(0, 1), &isec, &r, &s);
	ok = GeometryUtil::intersectionPoint(QPointF(-1, 0), QPointF(1, 0), QPointF(0, -3), QPointF(0, 1), &isec, &r, &s);

	ok = GeometryUtil::intersectionPoint(QPointF(-1, -1), QPointF(1, 1), QPointF(0, -3), QPointF(0, 1), &isec, &r, &s);
	*/

	RivmakerMainWindow w;
	w.show();
	return a.exec();
}
