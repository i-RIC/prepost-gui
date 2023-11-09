#include "gridcreatingconditionexternalprogram.h"
#include "gridcreatingconditionexternalprogramsettingdialog.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <misc/filesystemfunction.h>
#include <misc/mathsupport.h>
#include <misc/pythonutil.h>
#include <misc/stringtool.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProcess>
#include <QSettings>

#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>

GridCreatingConditionExternalProgram::GridCreatingConditionExternalProgram(const QString& folderName, const QLocale& locale, ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	m_definition {new SolverDefinition(folderName, locale)},
	m_name (m_definition->name()),
	m_locale {locale},
	m_rightClickingMenu {new QMenu()}
{
	setFilename("gridcreate.cgn");

	auto p = dynamic_cast<PreProcessorGridCreatingConditionDataItemI*>(parent);
	m_rightClickingMenu->addAction(p->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(p->clearAction());
}

GridCreatingConditionExternalProgram::~GridCreatingConditionExternalProgram()
{
	delete m_definition;
	delete m_rightClickingMenu;
}

bool GridCreatingConditionExternalProgram::create(QWidget* /*parent*/)
{
	QString fname = filename();
	QFile f(filename());
	if (! f.exists()) {
		QFileInfo finfo(f);
		QDir projDir(projectData()->workDirectory());
		QString relativePath = projDir.relativeFilePath(finfo.absolutePath());
		projDir.mkpath(relativePath);

		iRICLib::H5CgnsFile file(iRIC::toStr(fname), iRICLib::H5CgnsFile::Mode::Create);
	}
	GridCreatingConditionExternalProgramSettingDialog dialog(m_definition, m_locale, iricMainWindow(), preProcessorWindow());
	dialog.setFilename(fname);
	dialog.load();
	int ret = dialog.exec();
	if (ret != QDialog::Accepted) {return false;}

	// CGNS File OK. exexute the external program.
	QString externalProgram = m_definition->executableFilename();
	QFile prog(externalProgram);
	if (! prog.exists()) {
		// executable does not exists!!
		QMessageBox::critical(m_conditionDataItem->preProcessorWindow(), tr("Error"), tr("Grid generation program %1 does not exists.").arg(externalProgram));
		return false;
	}

	QProcess process(this);
	QString wd = projectData()->workDirectory();
	process.setWorkingDirectory(wd);
	auto env = projectData()->mainWindow()->processEnvironment();
	QStringList args;

	QFileInfo programInfo(externalProgram);
	if (programInfo.suffix() == "py" || programInfo.suffix() == "pyc") {
		QSettings settings;

		// run python solver
		QString pythonPath = settings.value("general/pythonpath", PythonUtil::defaultPath()).value<QString>();
		auto pythonPathStr = iRIC::toStr(pythonPath);

		QStringList args;
		args << "-u" << externalProgram << fname;

		// add path for python libraries
		QFileInfo finfo(pythonPath);
		auto pythonDir = finfo.dir();
		QStringList paths;
		paths.append(QDir::toNativeSeparators(pythonDir.absolutePath()));
		paths.append(QDir::toNativeSeparators(pythonDir.absoluteFilePath("Library\\mingw-w64\\bin")));
		paths.append(QDir::toNativeSeparators(pythonDir.absoluteFilePath("Library\\usr\\bin")));
		paths.append(QDir::toNativeSeparators(pythonDir.absoluteFilePath("Library\\bin")));
		paths.append(QDir::toNativeSeparators(pythonDir.absoluteFilePath("Scripts")));
		paths.append(QDir::toNativeSeparators(pythonDir.absoluteFilePath("bin")));
		QString path = env.value("PATH");
		path = paths.join(";") + ";" + path;
		env.insert("PATH", path);
		process.setProcessEnvironment(env);
		process.start(pythonPath, args);
	} else {
		process.setProcessEnvironment(env);
		args << fname;
		process.start(externalProgram, args);
	}

	WaitDialog waitDialog(preProcessorWindow());
	waitDialog.showProgressBar();
	waitDialog.setMessage(tr("Generating grid..."));
	waitDialog.setUnknownLimitMode(300);
	m_canceled = false;

	connect(&waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
	int progress = 3;
	// run for three seconds first.
	bool finished = process.waitForFinished(3000);

	if (! finished) {
		waitDialog.setProgress(progress);
		waitDialog.show();
	}
	qApp->processEvents();
	while (! finished && ! m_canceled) {
		finished = process.waitForFinished(1000);
		qApp->processEvents();
		waitDialog.setProgress(progress);
		++ progress;
	}
	waitDialog.hide();
	if (m_canceled) {
		// user canceled execution.
		return false;
	}

	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItemI*>(m_conditionDataItem->parent()->parent());
	auto gType = gtItem->gridType();
	v4InputGrid* grid = nullptr;

	// load create grid from cgnsfile. it is always loaded from the first zone in the first base.
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(fname), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		int ier = 0;

		int errorCode = 0;
		ier = file.ccBase()->readErrorCode(&errorCode);
		if (errorCode != 0) {
			QString msg = dialog.errorMessage(errorCode);
			QMessageBox::critical(preProcessorWindow(), tr("Error"), msg);
			return false;
		}

		auto firstZone = file.ccBase()->zoneById(1);
		if (firstZone == nullptr) {
			QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Grid Creation failed."));
			return false;
		}

		grid = v4InputGridIO::load(*firstZone, gType, offset(), &ier);
	}  catch (...) {
		QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Grid Creation failed."));
		return false;
	}

	emit gridCreated(grid);
	dataModel()->graphicsView()->cameraFit();
	return true;
}

void GridCreatingConditionExternalProgram::showInitialDialog()
{
	create(0);
}

bool GridCreatingConditionExternalProgram::ready() const
{
	return true;
}

void GridCreatingConditionExternalProgram::clear()
{
	GridCreatingConditionExternalProgramSettingDialog dialog(m_definition, m_locale, iricMainWindow(), preProcessorWindow());
	dialog.reset(true);
	dialog.save();
}

void GridCreatingConditionExternalProgram::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionExternalProgram::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

