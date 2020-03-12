#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingFreeTypeOpenGL);

#include <gui/main/iricmainwindow.h>
#include <gui/misc/periodicalupdatechecker.h>
#include <misc/errormessage.h>
#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QSplashScreen>
#include <QTextCodec>
#include <QtGlobal>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	// show splash screen.
	QPixmap pixmap(":/images/splashscreen.gif");
	QSplashScreen splash(pixmap);
	splash.show();
	a.processEvents();

	// set setting information.
	QCoreApplication::setOrganizationName("iRIC Organization");
	QCoreApplication::setOrganizationDomain("i-ric.org");
	QCoreApplication::setApplicationName("iRIC GUI 3");
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

	bool check = PeriodicalUpdateChecker::checkUpdate();
	if (check) {return 0;}

	auto udunitsPath = QDir::toNativeSeparators(QDir(qApp->applicationDirPath()).absoluteFilePath("udunits/udunits2.xml"));
	qputenv("UDUNITS2_XML_PATH", udunitsPath.toUtf8());

	try {
		iRICMainWindow w;
		w.show();
		splash.finish(&w);
		if (w.checkWorkFolderWorks()) {
			// work folder is good. project file can be opened.
			bool projectFound = false;
			if (argc > 1) {
				// project file or cgns file specified.
				QTextCodec* codec = QTextCodec::codecForLocale();
				int i = 1;
				while (i < argc && ! projectFound) {
					QString arg(argv[i]);
					if (arg.left(1) != "-") {
						// This is the project file name.
						QString projectFile = codec->toUnicode(argv[i]);
						QFileInfo finfo(projectFile);
						if (finfo.suffix() == "cgn") {
							// cgns file specified.
							w.importCalculationResult(projectFile);
						} else {
							// project file specified.
							w.openProject(projectFile);
						}
						projectFound = true;
					}
					++i;
				}
			}
			if (! projectFound) {
				w.openStartDialog();
			}
		}
		return a.exec();
	} catch (const ErrorMessage& msg) {
		QMessageBox::critical(&splash, iRICMainWindow::tr("Error"), msg);
		return 0;
	}
}
