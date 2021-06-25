#include "gridcreatingconditionexternalprogram.h"
#include "gridcreatingconditionexternalprogramsettingdialog.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/filesystemfunction.h>
#include <misc/mathsupport.h>
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

	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent);
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
		grid->loadFromCgnsFile(*firstZone);

		// apply offset
		auto o = offset();
		auto points = grid->vtkGrid()->GetPoints();
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			double v[3];
			points->GetPoint(i, v);
			v[0] -= o.x();
			v[1] -= o.y();
			points->SetPoint(i, v);
		}
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
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

