#include <vtkAutoInit.h>

#include <vtkVersionMacros.h>
#if (VTK_MAJOR_VERSION == 6) && (VTK_MINOR_VERSION == 1)
VTK_MODULE_INIT(vtkRenderingOpenGL);
#else
VTK_MODULE_INIT(vtkRenderingOpenGL2); // vtk 8.2
#endif
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
#if (VTK_MAJOR_VERSION == 6) && (VTK_MINOR_VERSION == 1)
VTK_MODULE_INIT(vtkRenderingFreeTypeOpenGL);
#endif

#include <gui/main/iricmainwindow.h>
#include <gui/misc/periodicalupdatechecker.h>
#include <misc/errormessage.h>
#include <misc/iricrootpath.h>
#include <misc/qttool.h>
#include <misc/stringtool.h>

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

#include <cstdlib>

int main(int argc, char* argv[])
{
	_putenv("QT_ENABLE_HIGHDPI_SCALING=1");

	QtTool::initNetworkAccessManager();
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication a(argc, argv);
	// show splash screen.
	QPixmap pixmap(":/images/splashscreen.gif");
	QSplashScreen splash(pixmap);
	splash.show();
	a.processEvents();

	// set setting information.
	QCoreApplication::setOrganizationName("iRIC Organization");
	QCoreApplication::setOrganizationDomain("i-ric.org");
	QCoreApplication::setApplicationName("iRIC GUI 4");
	// install translators.
	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();

	QString exeFolder = QApplication::applicationDirPath();
	iRICRootPath::set(exeFolder);

	QString proj_lib = QString("PROJ_LIB=") + QDir::toNativeSeparators(QDir(exeFolder).absoluteFilePath("share/proj"));
	_putenv(iRIC::toStr(proj_lib).c_str());

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

#ifdef WIN32
#include <windows.h>
// see https://stackoverflow.com/questions/55435230/how-to-ensure-directx-11-app-use-the-discrete-gpu-on-a-dual-gpu-laptop-with-c
// dumpbin /exports iRIC.exe | findstr "NvOptimusEnablement"
// dumpbin /exports iRIC.exe | findstr "AmdPowerXpressRequestHighPerformance"

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
// also see https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_6/nf-dxgi1_6-idxgifactory6-enumadapterbygpupreference
#endif

