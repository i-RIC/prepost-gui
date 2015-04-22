#include "../base/iricmainwindowinterface.h"
#include "../postcontainer/postsolutioninfo.h"
#include "../pre/base/preprocessorwindowinterface.h"
#include "../solverdef/solverdefinitionabstract.h"
#include "../pre/base/preprocessordatamodelinterface.h"
#include "backgroundimageinfo.h"
#include "cgnsfilelist.h"
#include "measureddata.h"
#include "projectcgnsfile.h"
#include "projectdata.h"
#include "projectdataitem.h"
#include "projectmainfile.h"
#include "projectpostprocessors.h"
#include "projectworkspace.h"
#include "offsetsettingdialog.h"
#include <guibase/coordinatesystemselectdialog.h>
#include <guicore/base/iricmainwindowinterface.h>

#include <cgnslib.h>
#include <cmath>
#include <iriclib.h>

#include <guibase/coordinatesystem.h>
#include <guibase/coordinatesystembuilder.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QRegExp>
#include <QSettings>
#include <QStatusBar>
#include <QStringList>
#include <QThread>
#include <QTime>
#include <QUrl>
#include <QXmlStreamWriter>
#include <QUndoCommand>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

const QString ProjectMainFile::FILENAME = "project.xml";
const QString ProjectMainFile::BGDIR = "backgroundimages";

ProjectMainFile::ProjectMainFile(ProjectData* parent)
	: ProjectDataItem(0)
{
	m_coordinateSystem = 0;
	m_offset = QVector2D(0, 0);
	m_isModified = false;
	m_isWaitigHttpResponse = false;
	m_projectData = parent;
	// @todo we should get iRIC version through parent->mainWindow(),
	// and set it to m_iRICVersion.
	m_cgnsFileList = new CgnsFileList(this);

	m_postSolutionInfo = new PostSolutionInfo(this);

	m_postProcessors = new ProjectPostProcessors(this);
}

ProjectMainFile::~ProjectMainFile()
{
	/// delete m_backgroundImages manually, so that background images destructor
	/// can access m_renderers.
	for (auto it = m_backgroundImages.begin(); it != m_backgroundImages.end(); ++it){
		delete *it;
	}

	/// delete m_postProcessors manually, so that post processors destructor
	/// can access mainwindow.
	delete m_postProcessors;

	// free coordinate system convert object
	if (m_coordinateSystem != 0){
		m_coordinateSystem->free();
	}
}

void ProjectMainFile::setModified()
{
	m_isModified = true;
}

void ProjectMainFile::createDefaultCgnsFile()
{
	QString defaultName = m_cgnsFileList->proposeFilename();
	m_cgnsFileList->add(defaultName);
	// create template CGNS file.
	QString fname = m_projectData->workCgnsFileName(defaultName);
	// @todo currently, cell_dim, phys_dim are hard-coded to be 2 fore each.
	// we must read it from Solver definition file in the future.
	ProjectCgnsFile::createNewFile(fname, 2, 2);
	ProjectCgnsFile::writeSolverInfo(fname, &(projectData()->solverDefinition()->abstract()));

	m_cgnsFileList->setCurrent(defaultName);
}

bool ProjectMainFile::switchCgnsFile(const QString& name)
{
	CgnsFileList::CgnsFileEntry* c = m_cgnsFileList->current();
	m_cgnsFileList->setCurrent(name);

	// save data to the old cgns file.
	if (c != 0 && c->filename() != name){
		bool ret = saveCgnsFile(c->filename());
		if (! ret){return false;}
	}

	// discard data loaded from cgns file.
	closeCgnsFile();

	// load data from the new cgns file.
	bool ok = loadCgnsFile(name);
	if (! ok){return false;}

	emit cgnsFileSwitched();
	return true;
}

void ProjectMainFile::loadSolverInformation()
{
	QString fname = filename();
	QFile f(fname);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);

	if (! ok){
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(fname).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	QDomElement element = doc.documentElement().toElement();
	m_iRICVersion = element.attribute("version", "1.0");
	m_solverName = element.attribute("solverName");
	m_solverVersion.fromString(element.attribute("solverVersion"));
}

void ProjectMainFile::initForSolverDefinition()
{
	SolverDefinition* def = m_projectData->solverDefinition();
	m_solverName = def->name();
	m_solverVersion = def->version();

	// create timesteps or iterationsteps depending on solver type.
	m_postSolutionInfo->setIterationType(def->iterationType());
}

QString ProjectMainFile::filename()
{
	return m_projectData->absoluteFileName(ProjectMainFile::FILENAME);
}

void ProjectMainFile::load()
{
	QDomDocument doc;
	loadDom(doc);

	loadFromProjectMainFile(doc.documentElement());
}

void ProjectMainFile::loadCgnsList()
{
	QString fname = filename();
	QFile f(fname);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);

	if (! ok){
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(fname).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	// read cgns file list
	QDomNode tmpNode = iRIC::getChildNode(doc.documentElement(), "CgnsFileList");
	if (! tmpNode.isNull())
	{
		m_cgnsFileList->loadFromProjectMainFile(tmpNode);
	}
}

bool ProjectMainFile::save()
{
	// save cgns file.
	bool ret = saveCgnsFile();
	if (! ret){return false;}
	ret = saveExceptCGNS();
	m_isModified = false;
	return ret;
}

bool ProjectMainFile::saveExceptCGNS()
{
	// save Project file.
	QString fname = filename();
	QFile f(fname);
	f.open(QFile::WriteOnly);
	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	// start xml
	w.writeStartDocument("1.0");
	w.writeStartElement("iRICProject");
	saveToProjectMainFile(w);
	w.writeEndElement();
	w.writeEndDocument();
	f.close();
	return true;
}

void ProjectMainFile::setSolverInformation(const SolverDefinitionAbstract& solver)
{
	m_solverName = solver.name();
	m_solverVersion = solver.version();
}

void ProjectMainFile::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement element = node.toElement();

	m_iRICVersion = element.attribute("version");
	checkVersionCompatibility();

	m_solverName = element.attribute("solverName");
	m_solverVersion.fromString(element.attribute("solverVersion"));

	// coordinate system
	QString coordName = element.attribute("coordinateSystem");
	m_coordinateSystem = projectData()->mainWindow()->coordinateSystemBuilder()->system(coordName);
	if (m_coordinateSystem != 0){
		m_coordinateSystem->init();
	}

	// coordinate offset
	double offsetX = iRIC::getDoubleAttribute(node, "offsetX");
	double offsetY = iRIC::getDoubleAttribute(node, "offsetY");
	m_offset.setX(offsetX);
	m_offset.setY(offsetY);

	// read cgns file list
	QDomNode tmpNode = iRIC::getChildNode(node, "CgnsFileList");
	if (! tmpNode.isNull())
	{
		m_cgnsFileList->loadFromProjectMainFile(tmpNode);
	}

	// read measured data
	tmpNode = iRIC::getChildNode(node, "MeasuredDatas");
	if (! tmpNode.isNull()){
		loadMeasuredDatas(tmpNode);
	}

	// read setting about Backgrounds
	tmpNode = iRIC::getChildNode(node, "Backgrounds");
	if (! tmpNode.isNull()){
		loadBackgrounds(tmpNode);
	}
	m_projectData->mainWindow()->loadSubWindowsFromProjectMainFile(node);
}

void ProjectMainFile::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("version", m_iRICVersion.toString());
	writer.writeAttribute("solverName", m_solverName);
	writer.writeAttribute("solverVersion", m_solverVersion.toString());

	if (m_coordinateSystem != 0){
		writer.writeAttribute("coordinateSystem", m_coordinateSystem->name());
	}

	iRIC::setDoubleAttribute(writer, "offsetX", m_offset.x());
	iRIC::setDoubleAttribute(writer, "offsetY", m_offset.y());

	// write cgns file list
	writer.writeStartElement("CgnsFileList");
	// delegate to CgnsFileList
	m_cgnsFileList->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write measured data
	writer.writeStartElement("MeasuredDatas");
	saveMeasuredDatas(writer);
	writer.writeEndElement();

	// write setting about PostProcessors
	writer.writeStartElement("PostProcessors");
	m_postProcessors->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write setting about Backgrounds
	writer.writeStartElement("Backgrounds");
	saveBackgrounds(writer);
	writer.writeEndElement();

	m_projectData->mainWindow()->saveSubWindowsToProjectMainFile(writer);
}

QStringList ProjectMainFile::containedFiles()
{
	QStringList ret;
	// Add files those exists in the work folder.
	QDir workDir(projectData()->workDirectory());
	QStringList files = workDir.entryList(QStringList(), QDir::Files, QDir::Name);
	for (int i = 0; i < files.size(); ++i){
		QString filename = files.at(i);
		if (filename == ProjectData::LOCKFILENAME){continue;}
		ret << filename;
	}
	// Add CGNS files.
	ret << m_cgnsFileList->containedFiles();

	// Add External files in MainWindow
	ret << m_projectData->mainWindow()->containedFiles();

	// Add Background image files
	ret << backgroundImageFiles();

	return ret;
}

void ProjectMainFile::importCgnsFile()
{
	QString fname = QFileDialog::getOpenFileName(
		m_projectData->mainWindow(), tr("Import CGNS file"), LastIODirectory::get(), tr("CGNS file (*.cgn)")
			);
	if (fname == ""){return;}
	importCgnsFile(fname);
}

void ProjectMainFile::importCgnsFile(const QString& fname)
{
	QString fnamebody = QFileInfo(fname).baseName();
	if (m_cgnsFileList->exists(fnamebody)){
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), QString(tr("Solution %1 already exists.").arg(fnamebody)));
		return;
	}
	QRegExp rx(ProjectCgnsFile::acceptablePattern());
	if (rx.indexIn(fnamebody) == - 1){
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), QString(tr("CGNS file whose name contains characters other than alphabets and numbers can not be imported.")));
		return;
	}

	m_cgnsFileList->add(fnamebody);
	QString to = m_projectData->workCgnsFileName(fnamebody);
	QFile::copy(fname, to);

	QString solverName;
	VersionNumber versionNumber;

	bool ret = ProjectCgnsFile::readSolverInfo(to, solverName, versionNumber);
	if (ret == true){
		if (m_solverName != solverName || (! m_solverVersion.compatibleWith(versionNumber))){
			QMessageBox::critical(m_projectData->mainWindow(), tr("Error"),
				tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver.").arg(solverName).arg(versionNumber.toString()));
			return;
		}
	}else{
		// error occured reading solver information.
		int ret = QMessageBox::warning(m_projectData->mainWindow(), tr("Warning"),
			tr("This CGNS file does not have solver information. "
				"We can not check whether this CGNS file is compatible with the solver. If it is not compatible, iRIC may crash.\nDo you really want to import this file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No){return;}
	}
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
	switchCgnsFile(fnamebody);

	// CGNS file import is not undo-able.
	iRICUndoStack::instance().clear();
}

bool ProjectMainFile::importCgnsFile(const QString& fname, const QString& newname)
{
	QString fnamebody = QFileInfo(newname).baseName();
	if (m_cgnsFileList->exists(fnamebody)){
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), QString(tr("Solution %1 already exists.").arg(fnamebody)));
		return false;
	}
	QRegExp rx(ProjectCgnsFile::acceptablePattern());
	if (rx.indexIn(fnamebody) == - 1){
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), QString(tr("CGNS file whose name contains characters other than alphabets and numbers can not be imported.")));
		return false;
	}

	m_cgnsFileList->add(fnamebody);
	QString to = m_projectData->workCgnsFileName(fnamebody);
	QFile::copy(fname, to);

	QString solverName;
	VersionNumber versionNumber;

	bool ret = ProjectCgnsFile::readSolverInfo(to, solverName, versionNumber);
	if (ret == true){
		if (m_solverName != solverName || (! m_solverVersion.compatibleWith(versionNumber))){
			QMessageBox::critical(m_projectData->mainWindow(), tr("Error"),
				tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver.").arg(solverName).arg(versionNumber.toString()));
			return false;
		}
	}else{
		// error occured reading solver information.
		int ret = QMessageBox::warning(m_projectData->mainWindow(), tr("Warning"),
			tr("This CGNS file does not have solver information. "
				"We can not check whether this CGNS file is compatible with the solver. If it is not compatible, iRIC may crash.\nDo you really want to import this file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No){return false;}
	}
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
	switchCgnsFile(fnamebody);

	// CGNS file import is not undo-able.
	iRICUndoStack::instance().clear();
	return true;
}

void ProjectMainFile::exportCurrentCgnsFile()
{
	QString fname = QFileDialog::getSaveFileName(
		m_projectData->mainWindow(), tr("Export CGNS file"), LastIODirectory::get(), tr("CGNS file (*.cgn)")
			);
	if (fname == ""){return;}
	if (QFile::exists(fname)){
		// if the specified file already exests, remove it first.
		if (! QFile::remove(fname)){
			QMessageBox::warning(m_projectData->mainWindow(), tr("Error"), tr("Exporting CGNS file failed. Specified file is locked by other process."));
			return;
		}
	}
	// save first.
	saveCgnsFile();
	// copy to the specified file.
	QString from = m_projectData->workCgnsFileName(m_cgnsFileList->current()->filename());
	QFile::copy(from, fname);
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

const QString ProjectMainFile::currentCgnsFileName() const
{
	return m_projectData->workCgnsFileName(m_cgnsFileList->current()->filename());
}

bool ProjectMainFile::loadCgnsFile(const QString& name)
{
	// CGNS file name
	QString fname = m_projectData->workCgnsFileName(name);
	// save to current cgns file.
	int fn;
	int ret;
	ret = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_READ, &fn);
	if (ret != 0){
		QString shortFilename = name;
		shortFilename.append(".cgn");
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening CGNS file in project file : %1").arg(shortFilename));
		return false;
	}
	// load data.
	loadFromCgnsFile(fn);
	cg_close(fn);
	return true;
}

bool ProjectMainFile::saveCgnsFile()
{
	return saveCgnsFile(m_cgnsFileList->current()->filename());
}

bool ProjectMainFile::saveCgnsFile(const QString& name)
{
	QTime time;
	// close CGNS file when the solution opened it.
	m_postSolutionInfo->close();

	// check grid status
	try {
		if (! clearResultsIfGridIsEdited()){
			return false;
		}
	} catch (ErrorMessage& m){
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("%1 Saving project file failed.").arg(m));
		return false;
	}
	// close CGNS file when the solution opened it, again.
	m_postSolutionInfo->close();

	// CGNS file name
	QString fname = m_projectData->workCgnsFileName(name);
	// save to current cgns file.
	int fn, ier;
	time.start();
	ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_MODIFY, &fn);
	qDebug("cg_open(): %d", time.elapsed());
	if (ier != 0){return false;}
	// write solver information
	bool ret = ProjectCgnsFile::writeSolverInfo(fn, &(m_projectData->solverDefinition()->abstract()));
	if (ret == false){goto ERROR;}
	// save data.
	time.start();
	try {
		saveToCgnsFile(fn);
	} catch (ErrorMessage& m){
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("%1 Saving project file failed.").arg(m));
		goto ERROR;
	}
	qDebug("saveToCgnsFile(): %d", time.elapsed());
	time.start();
	ier = cg_configure(CG_CONFIG_COMPRESS, reinterpret_cast<void*>(1));
	if (ier != 0){goto ERROR;}
	cg_close(fn);
	qDebug("cg_close(): %d", time.elapsed());
	return true;
ERROR:
	cg_close(fn);
	return false;
}

void ProjectMainFile::loadFromCgnsFile(const int fn)
{
	// Init iriclib for reading
	cg_iRIC_InitRead(fn);
	m_projectData->mainWindow()->loadFromCgnsFile(fn);
	m_postSolutionInfo->loadFromCgnsFile(fn);
}

void ProjectMainFile::saveToCgnsFile(const int fn)
{
	m_projectData->mainWindow()->saveToCgnsFile(fn);
}

void ProjectMainFile::toggleGridEditFlag()
{
	m_projectData->mainWindow()->toggleGridEditFlag();
}

void ProjectMainFile::closeCgnsFile()
{
	m_projectData->mainWindow()->closeCgnsFile();
	m_postSolutionInfo->closeCgnsFile();
}

void ProjectMainFile::clearResults()
{
	// close CGNS file when the solution opened it.
	m_postSolutionInfo->close();

	// CGNS file name
	QString fname = currentCgnsFileName();
	// save to current cgns file, with write mode.
	ProjectCgnsFile::createNewFile(fname, 2, 2);
	int fn, ier;
	ier = cg_open(iRIC::toStr(fname).c_str(), CG_MODE_MODIFY, &fn);
	if (ier != 0){return;}
	// write solver information
	bool ret = ProjectCgnsFile::writeSolverInfo(fn, &(m_projectData->solverDefinition()->abstract()));
	if (ret == false){goto ERROR;}
	// save data.
	toggleGridEditFlag();
	saveToCgnsFile(fn);
	cg_close(fn);
	m_postSolutionInfo->checkCgnsStepsUpdate();
	projectData()->mainWindow()->clearResults();
	return;

ERROR:
	cg_close(fn);
	return;
}

bool ProjectMainFile::hasResults()
{
	return m_postSolutionInfo->hasResults();
}

void ProjectMainFile::addBackgroundImage()
{
	QString dir = LastIODirectory::get();
	QString filter(tr("All images(*.jpg *.jpeg *.png *.tif);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png);;TIFF images(*.tif)"));
	QMdiArea* mdiArea = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QString fname = QFileDialog::getOpenFileName(mdiArea->currentSubWindow(), tr("Open Image file"), dir, filter);
	if (fname == "") return;

	QFileInfo finfo(fname);
	// check whether a image file with the same filename exists.
	QString filename = finfo.fileName();
	for (int i = 0; i < m_backgroundImages.count(); ++i){
		BackgroundImageInfo* imgInfo = m_backgroundImages.at(i);
		if (imgInfo->fileName().toLower() == filename.toLower()){
			// file with the same name already exists.
			QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("A background image with the same name already exists."));
			return;
		}
	}

	// copy to the project file.
	if (! mkdirBGDIR()){
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("The background image was not added. Please try again."));
		return;
	}
	QDir wkDir = QDir(projectData()->workDirectory());

	QString to = wkDir.absoluteFilePath(QFileInfo(fname).fileName());
	if (! wkDir.exists(QFileInfo(fname).fileName())){
		if (! QFile::copy(fname, to)){
			QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("The background image was not added. Please try again."));
			return;
		}
	}

	QString ext = finfo.suffix().toLower();
	if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "tif"){
		try {
			BackgroundImageInfo* image = new BackgroundImageInfo(to, fname, this);
			addBackgroundImage(image);
		} catch (ErrorMessage m){
			QMessageBox::warning(iricMainWindow(), tr("Warning"), m);
		}
	}else{
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("Invalid image file is specified."));
	}
}

void ProjectMainFile::addBackgroundImage(BackgroundImageInfo* image)
{
	m_backgroundImages.push_front(image);
	emit backgroundImageAdded();
	setModified();
}

bool ProjectMainFile::mkdirBGDIR()
{
	QDir wkDir = QDir(projectData()->workDirectory());
	if (wkDir.cd(BGDIR)){
		// It already exists.
		return true;
	}

	// create directory.
	wkDir.mkdir(BGDIR);
	if (! wkDir.cd(BGDIR)){
		return false;
	}
	return true;
}

void ProjectMainFile::deleteImage(const QModelIndex &index)
{
	auto it = m_backgroundImages.begin();
	BackgroundImageInfo* image = *(it + index.row());
	QString fname = image->caption();
	m_backgroundImages.erase(it + index.row());
	delete image;
	emit backgroundImageDeleted(index.row());

	// remove background image file from project file.
	bool del = true;
	for (it = m_backgroundImages.begin(); it != m_backgroundImages.end(); ++it){
		BackgroundImageInfo* info = *it;
		if (fname != info->caption()) continue;
		del = false;
		break;
	}
	if (del){
		QDir wkDir = QDir(projectData()->workDirectory());
		wkDir.cd(BGDIR);
		wkDir.remove(fname);
	}

	// removing image is not undo-able.
	iRICUndoStack::instance().clear();
	setModified();
}

void ProjectMainFile::moveUpImage(const QModelIndex &index)
{
	if (index.row() == 0){
		// Can not move up
		return;
	}
	auto it = m_backgroundImages.begin();
	int i = m_backgroundImages.indexOf(*(it + index.row()));
	int j = m_backgroundImages.indexOf(*(it + index.row() - 1));
	m_backgroundImages.swap(i, j);
	emit backgroundImageMovedUp(index.row());
	setModified();
}

void ProjectMainFile::moveDownImage(const QModelIndex &index)
{
	if (index.row() == m_backgroundImages.size() - 1){
		// Can not move down
		return;
	}
	auto it = m_backgroundImages.begin();
	int i = m_backgroundImages.indexOf(*(it + index.row()));
	int j = m_backgroundImages.indexOf(*(it + index.row() + 1));
	m_backgroundImages.swap(i, j);
	emit backgroundImageMovedDown(index.row());
	setModified();
}

void ProjectMainFile::deleteMeasuredData(const QModelIndex &index)
{
	auto it = m_measuredDatas.begin();
	MeasuredData* md = *(it + index.row());
	m_measuredDatas.erase(it + index.row());
	delete md;
	emit measuredDataDeleted(index.row());

	// removing image is not undo-able.
	iRICUndoStack::instance().clear();
	setModified();
}

void ProjectMainFile::moveUpMeasuredData(const QModelIndex &index)
{
	if (index.row() == 0){
		// Can not move up
		return;
	}
	auto it = m_backgroundImages.begin();
	int i = m_backgroundImages.indexOf(*(it + index.row()));
	int j = m_backgroundImages.indexOf(*(it + index.row() - 1));
	m_backgroundImages.swap(i, j);
	emit measuredDataMovedUp(index.row());
	setModified();
}

void ProjectMainFile::moveDownMeasuredData(const QModelIndex &index)
{
	if (index.row() == m_measuredDatas.size() - 1){
		// Can not move down
		return;
	}
	auto it = m_measuredDatas.begin();
	int i = m_measuredDatas.indexOf(*(it + index.row()));
	int j = m_measuredDatas.indexOf(*(it + index.row() + 1));
	m_measuredDatas.swap(i, j);
	emit measuredDataMovedDown(index.row());
	setModified();
}

void ProjectMainFile::addRenderer(vtkRenderer *ren)
{
	for (auto it = m_renderers.begin(); it != m_renderers.end(); ++it){
		if (*it == ren) return;
	}
	m_renderers.append(ren);
}

void ProjectMainFile::removeRenderer(vtkRenderer *ren)
{
	for (auto it = m_renderers.begin(); it != m_renderers.end(); ++it){
		if (*it == ren){
			m_renderers.erase(it);
			return;
		}
	}
}

void ProjectMainFile::checkVersionCompatibility()
{
	VersionNumber iRICVer = iricMainWindow()->versionNumber();
	VersionNumber projVer = m_iRICVersion;
	if (iRICVer.compatibleWith(projVer)){
		// OK, no problem.
		return;
	}
	// check for exceptional cases.

	// if iRIC ver. is 1.9 and project ver. is 2.0, it is accepted.
	if (iRICVer.major() == 1 && iRICVer.minor() == 9 && projVer.major() == 2 && projVer.minor() == 0){
		return;
	}
	// if iRIC ver. is 2.0 and project ver. is 1.9, it is accepted.
	if (iRICVer.major() == 2 && iRICVer.minor() == 0 && projVer.major() == 1 && projVer.minor() == 9){
		return;
	}

	// error.
	if (iRICVer.major() > projVer.major()){
		throw ErrorMessage(tr("This project file cannot be read, because it was created by too old iRIC (version %1).").arg(projVer.toString()));
	}else{
		throw ErrorMessage(tr("This project file cannot be read, because it was created by newer iRIC (version %1).").arg(projVer.toString()));
	}
}

void ProjectMainFile::loadBackgrounds(const QDomNode& node)
{
	for (int i = 0; i < m_backgroundImages.size(); ++i){
		delete m_backgroundImages.at(i);
	}
	m_backgroundImages.clear();
	QDomNodeList children = node.childNodes();
	int len = children.count();
	for (int i = 0; i < len; ++i){
		QDomNode child = children.at(len - 1 - i);
		QDomElement elem = child.toElement();
		QString fname = elem.attribute("filename");
		QString absolutePath = projectData()->workDirectory();
		absolutePath.append("/").append(BGDIR).append("/").append(fname);
		QFileInfo finfo(absolutePath);
		if (finfo.exists()){
			try {
				BackgroundImageInfo* image = new BackgroundImageInfo(absolutePath, absolutePath, this);
				m_backgroundImages.push_front(image);
				image->loadFromProjectMainFile(child);
				emit backgroundImageAdded();
			} catch (ErrorMessage m){
				QMessageBox::warning(iricMainWindow(), tr("Warning"), m);
			}
		} else {
			QString text = absolutePath;
			text.append(tr(" : no such file."));
			QMessageBox::warning(iricMainWindow(), tr("Warning"), text);
		}
	}
}

void ProjectMainFile::saveBackgrounds(QXmlStreamWriter& writer)
{
	for (auto it = m_backgroundImages.begin(); it != m_backgroundImages.end(); ++it){
		BackgroundImageInfo* bg = *it;
		writer.writeStartElement("BackgroundImageInfo");
		bg->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}


void ProjectMainFile::loadMeasuredDatas(const QDomNode& node)
{
	for (int i = 0; i < m_measuredDatas.size(); ++i){
		delete m_measuredDatas.at(i);
	}
	m_measuredDatas.clear();
	QDomNodeList children = node.childNodes();
	int len = children.count();
	for (int i = 0; i < len; ++i){
		QDomNode child = children.at(len - 1 - i);
		MeasuredData* md = new MeasuredData(this);
		m_measuredDatas.push_front(md);
		md->loadFromProjectMainFile(child);
	}
}

void ProjectMainFile::saveMeasuredDatas(QXmlStreamWriter& writer)
{
	int index = 1;
	for (auto it = m_measuredDatas.begin(); it != m_measuredDatas.end(); ++it){
		MeasuredData* md = *it;
		md->setIndex(index);
		writer.writeStartElement("MeasuredData");
		md->saveToProjectMainFile(writer);
		writer.writeEndElement();
		++ index;
	}
}

void ProjectMainFile::updateActorVisibility(int idx, bool vis)
{
	if (idx < 0){} else {
		auto it = m_backgroundImages.begin();
		BackgroundImageInfo* bg = *(it + idx);
		bg->setVisible(vis);
	}
	emit backgroundActorVisibilityChanged(idx, vis);
}

QStringList ProjectMainFile::backgroundImageFiles()
{
	QStringList ret;

	QDir dir(projectData()->workDirectory());
	if (! dir.cd(BGDIR)) return ret;

	QStringList list = dir.entryList(QDir::Files);
	for (auto it = list.begin(); it != list.end(); ++it){
		QString tmp = *it;
		QString path(BGDIR);
		path.append("/").append(tmp);
		ret << path;
	}

	return ret;
}

bool ProjectMainFile::clearResultsIfGridIsEdited()
{
	return m_projectData->mainWindow()->clearResultsIfGridIsEdited();
}

void ProjectMainFile::addMeasuredData()
{
	QString dir = LastIODirectory::get();
	QString filter(tr("Text Files (*.csv *.txt);;All Files (*.*)"));
	QString fname = QFileDialog::getOpenFileName(iricMainWindow(), tr("Open Measured Data File"), dir, filter);
	if (fname == "") return;

	QFileInfo finfo(fname);
	try {
		MeasuredData* md = new MeasuredData(this);
		md->importFromFile(fname);
		m_measuredDatas.append(md);
		emit measuredDataAdded();
		setModified();
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message){
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}

void ProjectMainFile::openPostProcessors()
{
	QDomDocument doc;
	loadDom(doc);

	QDomElement docElem = doc.documentElement();

	// read setting about Post Processors
	// Post processor settings are loaded later.
	QDomNode tmpNode = iRIC::getChildNode(docElem, "PostProcessors");
	if (! tmpNode.isNull()){
		m_postProcessors->loadFromProjectMainFile(tmpNode);
	}
}

void ProjectMainFile::loadDom(QDomDocument& doc) {
	QString fname = filename();
	QFile f(fname);
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok){
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(fname).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
}

bool ProjectMainFile::importVisGraphSetting(const QString filename)
{
	QFile f(filename);

	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok){
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("Error occured while loading %1.").arg(filename));
		return false;
	}
	QDomElement elem = doc.documentElement();
	QString solvername = elem.attribute("solverName");
	QString solverVersion = elem.attribute("solverVersion");
	VersionNumber vn(solverVersion);

	if (solvername != m_solverName || ! m_solverVersion.compatibleWith(vn)){
		int ret = QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("This file is for solver %1 %2. It is not compatible with the solver you are using, so maybe importing this file will fail. Do you really want to import this file?").arg(solvername).arg(solverVersion), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No){return false;}
	}
	m_postProcessors->loadFromProjectMainFile(doc.documentElement(), true);

	return true;
}

bool ProjectMainFile::exportVisGraphSetting(const QString filename)
{
	if (m_postProcessors->windowCount() == 0){
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("There is no visualization/graph windows."));
		return false;
	}
	QFile f(filename);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok){
		// Failed opening the file.
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("File %1 could not be opened.").arg(filename));
		return false;
	}
	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	w.writeStartDocument("1.0");
	w.writeStartElement("iRICPostProcessingSettings");
	w.writeAttribute("solverName", m_solverName);
	w.writeAttribute("solverVersion", m_solverVersion.toString());
	m_postProcessors->saveToProjectMainFile(w);
	w.writeEndElement();
	w.writeEndDocument();
	f.close();

	return true;
}

void ProjectMainFile::setCoordinateSystem(CoordinateSystem* system)
{
	if (m_coordinateSystem != 0){
		m_coordinateSystem->free();
	}
	m_coordinateSystem = system;
	if (m_coordinateSystem != 0){m_coordinateSystem->init();}
}

void ProjectMainFile::setOffset(double x, double y)
{
	double x_diff = x - m_offset.x();
	double y_diff = y - m_offset.y();
	projectData()->mainWindow()->preProcessorWindow()->dataModel()->applyOffset(x_diff, y_diff);
	m_offset.setX(x);
	m_offset.setY(y);
}

class ProjectSetOffsetCommand : public QUndoCommand
{
public:
	ProjectSetOffsetCommand(const QVector2D& newoffset, ProjectMainFile* file){
		m_newOffset = newoffset;
		m_oldOffset = file->offset();
		m_projectMainFile = file;
	}
	void redo(){
		m_projectMainFile->setOffset(m_newOffset.x(), m_newOffset.y());
	}
	void undo(){
		m_projectMainFile->setOffset(m_oldOffset.x(), m_oldOffset.y());
	}


private:
	QVector2D m_newOffset;
	QVector2D m_oldOffset;
	ProjectMainFile* m_projectMainFile;
};

void ProjectMainFile::setupOffset()
{
	OffsetSettingDialog dialog;
	dialog.setOffset(m_offset.x(), m_offset.y());
	int ret = dialog.exec();
	if (ret == QDialog::Rejected){return;}

	iRICUndoStack::instance().push(new ProjectSetOffsetCommand(QVector2D(dialog.offsetX(), dialog.offsetY()), this));
}

int ProjectMainFile::showCoordinateSystemDialog()
{
	iRICMainWindowInterface* mainW = projectData()->mainWindow();
	CoordinateSystemSelectDialog dialog(mainW);
	dialog.setBuilder(mainW->coordinateSystemBuilder());
	dialog.setCoordinateSystem(coordinateSystem());
	int ret = dialog.exec();
	if (ret == QDialog::Rejected){return ret;}
	setCoordinateSystem(dialog.coordinateSystem());
	return ret;
}
