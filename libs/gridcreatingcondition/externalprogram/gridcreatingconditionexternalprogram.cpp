#include "gridcreatingconditionexternalprogram.h"
#include "gridcreatingconditionexternalprogramsettingdialog.h"
//#include "pre/preprocessorwindow.h"
//#include "project/projectdata.h"

#include <guibase/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProcess>

#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <cgnslib.h>
#include <iriclib.h>

GridCreatingConditionExternalProgram::GridCreatingConditionExternalProgram(const QString& folderName, const QLocale& locale, ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
	m_definition = new SolverDefinition(folderName, locale);
	m_locale = locale;
	m_name = m_definition->name();
	m_filename = "gridcreate.cgn";

	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent);
	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(p->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(p->clearAction());
}

GridCreatingConditionExternalProgram::~GridCreatingConditionExternalProgram()
{
	delete m_definition;
	if (m_rightClickingMenu != nullptr) {
		delete m_rightClickingMenu;
	}
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
		ProjectCgnsFile::createNewFile(fname, 2, 2);
	}
	GridCreatingConditionExternalProgramSettingDialog dialog(m_definition, m_locale, iricMainWindow(), preProcessorWindow());
	dialog.setFilename(fname);
	dialog.load();
	int ret = dialog.exec();
	if (ret != QDialog::Accepted) {return false;}

	// delete grid.
	deleteGrid(fname);

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
	process.setProcessEnvironment(projectData()->mainWindow()->processEnvironment());
	QStringList args;
	args << fname;

	WaitDialog waitDialog(preProcessorWindow());
	waitDialog.showProgressBar();
	waitDialog.setMessage(tr("Generating grid..."));
	waitDialog.setUnknownLimitMode(300);
	m_canceled = false;

	connect(&waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
	process.start(externalProgram, args);
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

	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	SolverDefinitionGridType* gType = gtItem->gridType();
	// Create grid.
	Grid* grid = gType->createEmptyGrid();
	gType->buildGridRelatedConditions(grid);

	// load create grid from cgnsfile. it is always loaded from the first zone in the first base.
	int fn;
	ret = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &fn);
	// load error code first.
	int errorCode = readErrorCode(fn);
	if (errorCode != 0) {
		cg_close(fn);
		// error occured while grid is generated.
		QString msg = dialog.errorMessage(errorCode);
		QMessageBox::critical(preProcessorWindow(), tr("Error"), msg);
		return false;
	}
	bool ok = grid->loadFromCgnsFile(fn, 1, 1);
	cg_close(fn);
	if (! ok) {
		// grid generator did not create grid correctly.
		QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Grid Creation failed."));
		return false;
	}

	emit gridCreated(grid);
	dataModel()->fit();
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

void GridCreatingConditionExternalProgram::deleteGrid(const QString& fname)
{
	int ret;
	int fn;
	int B;
	ret = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_MODIFY, &fn);
	cg_iRIC_GotoBase(fn, &B);
	QStringList zoneNames;
	int nzones;
	ret = cg_nzones(fn, B, &nzones);
	for (int Z = 1; Z <= nzones; ++Z) {
		char zonename[ProjectCgnsFile::BUFFERLEN];
		cgsize_t size[9];
		cg_zone_read(fn, B, Z, zonename, &(size[0]));
		zoneNames.append(zonename);
	}
	// zone names are known, so remove them.
	cg_goto(fn, B, "end");
	for (const QString& zoneName : zoneNames) {
		cg_delete_node(const_cast<char*>(iRIC::toStr(zoneName).c_str()));
	}
	cg_close(fn);
}

int GridCreatingConditionExternalProgram::readErrorCode(int fn)
{
	int ret;
	DataType_t dataType;
	int dim;
	cgsize_t dimVec[3];
	char tmpname[ProjectCgnsFile::BUFFERLEN];
	ret = cg_goto(fn, 1, "ErrorCode", 0, "end");
	if (ret != 0) {
		// no error code.
		return 0;
	}

	int narrays;
	ret = cg_narrays(&narrays);
	if (ret != 0) {return 0;}
	if (narrays < 1) {return 0;}
	cg_array_info(1, tmpname, &dataType, &dim, &(dimVec[0]));
	if (QString(tmpname) != "Value") {return 0;}
	int errorcode;
	cg_array_read(1, &errorcode);
	return errorcode;
}

void GridCreatingConditionExternalProgram::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionExternalProgram::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::RightButton) {
		if (isNear(m_dragStartPoint, QPoint(event->x(), event->y()))) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

