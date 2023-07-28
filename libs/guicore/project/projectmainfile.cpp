#include "../base/iricmainwindowinterface.h"
#include "../postcontainer/postbaseiterativevaluescontainer.h"
#include "../postcontainer/postsolutioninfo.h"
#include "../pre/base/preprocessordatamodelinterface.h"
#include "../pre/base/preprocessorwindowinterface.h"
#include "../solverdef/solverdefinitionabstract.h"
#include "backgroundimageinfo.h"
#include "measured/measureddata.h"
#include "measured/measureddatacsvimporter.h"
#include "offsetsettingdialog.h"
#include "projectcgnsfile.h"
#include "projectcgnsmanager.h"
#include "projectdata.h"
#include "projectdataitem.h"
#include "projectmainfile.h"
#include "projectpostprocessors.h"
#include "projectworkspace.h"
#include "private/projectmainfile_impl.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemselectdialog.h>
#include <guibase/timeformat/timeformatutil.h>
#include <misc/errormessage.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/valuechangert.h>
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
#include <QUndoCommand>
#include <QUrl>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <h5cgnsfile.h>
#include <h5cgnsfileseparatesolutionutil.h>
#include <iriclib_errorcodes.h>

#include <cmath>

namespace {

void copyCgnsFile(const QString& from, const QString& to)
{
	// copy the CGNS itself
	QFile::copy(from, to);

	QFileInfo from_info(from);
	QFileInfo to_info(to);

	auto from_result = from_info.absoluteDir().absoluteFilePath("result");
	auto to_result = to_info.absoluteDir().absoluteFilePath("result");

	QFile from_result_file(from_result);
	if (! from_result_file.exists()) {return;}

	iRIC::copyDirRecursively(from_result, to_result);
}

} // namespace

const QString ProjectMainFile::FILENAME = "project.xml";
const QString ProjectMainFile::BGDIR = "backgroundimages";

ProjectMainFile::Impl::Impl(ProjectMainFile *parent) :
	m_cgnsManager {new ProjectCgnsManager(parent)},
	m_postSolutionInfo {new PostSolutionInfo(parent)},
	m_postProcessors {new ProjectPostProcessors(parent)},
	m_coordinateSystem {nullptr},
	m_zeroDateTime {},
	m_timeZone {QTimeZone::utc()},
	m_timeFormat {TimeFormat::elapsed_SS_sec},
	m_showTimeZone {false},
	m_separateResult {false},
	m_offset {QPointF(0, 0)},
	m_isModified {false},
	m_cgnsFile {nullptr},
	m_parent {parent}
{}

ProjectMainFile::Impl::~Impl()
{
	if (m_coordinateSystem != nullptr) {
		m_coordinateSystem->free();
	}
	delete m_cgnsManager;
	delete m_postProcessors;
	delete m_postSolutionInfo;
}

void ProjectMainFile::Impl::loadMeasuredDatas(const QDomNode& node)
{
	clearMeasuredDatas();

	QDomNodeList children = node.childNodes();
	int len = children.count();
	for (int i = 0; i < len; ++i) {
		QDomNode child = children.at(len - 1 - i);
		MeasuredData* md = new MeasuredData(m_parent);
		m_measuredDatas.insert(m_measuredDatas.begin(), md);
		md->loadFromProjectMainFile(child);
	}
}

void ProjectMainFile::Impl::saveMeasuredDatas(QXmlStreamWriter& writer)
{
	int index = 1;
	for (MeasuredData* md : m_measuredDatas) {
		md->setIndex(index);
		writer.writeStartElement("MeasuredData");
		md->saveToProjectMainFile(writer);
		writer.writeEndElement();
		++ index;
	}
}

void ProjectMainFile::Impl::clearMeasuredDatas()
{
	for (auto d : m_measuredDatas) {
		delete d;
	}
	m_measuredDatas.clear();
}

void ProjectMainFile::Impl::loadBackgrounds(const QDomNode& node)
{
	clearBackgroundImages();

	QDomNodeList children = node.childNodes();
	int len = children.count();
	for (int i = 0; i < len; ++i) {
		QDomNode child = children.at(len - 1 - i);
		QDomElement elem = child.toElement();
		QString fname = elem.attribute("filename");
		QString absolutePath = m_parent->projectData()->workDirectory();
		absolutePath.append("/").append(BGDIR).append("/").append(fname);
		QFileInfo finfo(absolutePath);
		if (finfo.exists()) {
			try {
				BackgroundImageInfo* image = new BackgroundImageInfo(absolutePath, m_parent);
				m_backgroundImages.insert(m_backgroundImages.begin(), image);
				image->loadFromProjectMainFile(child);
				emit m_parent->backgroundImageAdded();
			} catch (ErrorMessage m) {
				QMessageBox::warning(m_parent->iricMainWindow(), tr("Warning"), m);
			}
		} else {
			QString text = absolutePath;
			text.append(tr(" : no such file."));
			QMessageBox::warning(m_parent->iricMainWindow(), tr("Warning"), text);
		}
	}
}

void ProjectMainFile::Impl::saveBackgrounds(QXmlStreamWriter& writer)
{
	for (BackgroundImageInfo* i : m_backgroundImages) {
		writer.writeStartElement("BackgroundImageInfo");
		i->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QStringList ProjectMainFile::Impl::backgroundImageFiles() const
{
	QStringList ret;

	QDir dir(m_parent->projectData()->workDirectory());
	if (! dir.cd(BGDIR)) {return ret;}

	QStringList list = dir.entryList(QDir::Files);
	for (auto it = list.begin(); it != list.end(); ++it) {
		QString tmp = *it;
		QString path(BGDIR);
		path.append("/").append(tmp);
		ret << path;
	}

	return ret;
}

void ProjectMainFile::Impl::clearBackgroundImages()
{
	for (auto i : m_backgroundImages) {
		delete i;
	}
	m_backgroundImages.clear();
}

// public interfaces

ProjectMainFile::ProjectMainFile(ProjectData* parent) :
	ProjectDataItem(0),
	impl {new Impl(this)}
{
	m_projectData = parent;
}

ProjectMainFile::~ProjectMainFile()
{
	impl->clearBackgroundImages();
	impl->clearMeasuredDatas();

	delete impl;
}

void ProjectMainFile::setModified(bool modified)
{
	impl->m_isModified = modified;
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

	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(fname).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	QDomElement element = doc.documentElement().toElement();
	impl->m_iRICVersion = element.attribute("version", "1.0");
	impl->m_solverName = iRIC::toStr(element.attribute("solverName"));
	impl->m_solverVersion = element.attribute("solverVersion");
}

void ProjectMainFile::createMainCgnsFile()
{
	auto fname = impl->m_cgnsManager->mainFileFullName();
	iRICLib::H5CgnsFile cgnsFile(fname, iRICLib::H5CgnsFile::Mode::Create);
	ProjectCgnsFile::writeSolverInfo(&cgnsFile, projectData()->solverDefinition()->abstract());
}

void ProjectMainFile::initForSolverDefinition()
{
	SolverDefinition* def = m_projectData->solverDefinition();
	impl->m_solverName = def->name();
	impl->m_solverVersion = def->version();

	// create timesteps or iterationsteps depending on solver type.
	impl->m_postSolutionInfo->setIterationType(def->iterationType());
}

QString ProjectMainFile::filename() const
{
	return m_projectData->absoluteFileName(ProjectMainFile::FILENAME);
}

QString ProjectMainFile::workDirectory() const
{
	return m_projectData->workDirectory();
}

void ProjectMainFile::load()
{
	QDomDocument doc;
	loadDom(doc);

	loadFromProjectMainFile(doc.documentElement());
}

bool ProjectMainFile::save()
{
	// save cgns file.
	int ret = saveToCgnsFile();
	if (ret != IRIC_NO_ERROR) {return false;}

	ret = saveExceptCGNS();
	impl->m_isModified = false;
	return ret;
}

bool ProjectMainFile::saveExceptCGNS()
{
	// close CGNS file when the solution opened it.
	impl->m_postSolutionInfo->close();

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
	impl->m_solverName = solver.name();
	impl->m_solverVersion = solver.version();
}

const std::string& ProjectMainFile::solverName() const
{
	return impl->m_solverName;
}

void ProjectMainFile::setSolverName(const std::string& name)
{
	impl->m_solverName = name;
}

const VersionNumber& ProjectMainFile::solverVersion() const
{
	return impl->m_solverVersion;
}

void ProjectMainFile::setSolverVersion(const VersionNumber& version)
{
	impl->m_solverVersion = version;
}

const VersionNumber& ProjectMainFile::iRICVersion() const
{
	return impl->m_iRICVersion;
}

void ProjectMainFile::setIRICVersion(const VersionNumber& v)
{
	impl->m_iRICVersion = v;
}

void ProjectMainFile::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement element = node.toElement();

	impl->m_iRICVersion = element.attribute("version");
	checkVersionCompatibility();

	impl->m_solverName = iRIC::toStr(element.attribute("solverName"));
	impl->m_solverVersion = element.attribute("solverVersion");

	// coordinate system
	QString coordName = element.attribute("coordinateSystem");
	impl->m_coordinateSystem = projectData()->mainWindow()->coordinateSystemBuilder()->system(coordName);
	if (impl->m_coordinateSystem != nullptr) {
		impl->m_coordinateSystem->init();
	}

	// zero data time
	QString dt = element.attribute("zeroDateTime", "");
	if (! dt.isEmpty()) {
		impl->m_zeroDateTime = QDateTime::fromString(dt, Qt::ISODate);
	}
	// time zone
	impl->m_timeZone = QTimeZone(element.attribute("timeZoneOffsetSeconds", "0").toInt());

	// time format
	QString f = element.attribute("timeFormat", "");
	impl->m_timeFormat = TimeFormatUtil::fromString(f);
	impl->m_customTimeFormat = element.attribute("customTimeFormat", "");

	// coordinate offset
	double offsetX = iRIC::getDoubleAttribute(node, "offsetX");
	double offsetY = iRIC::getDoubleAttribute(node, "offsetY");
	impl->m_offset.setX(offsetX);
	impl->m_offset.setY(offsetY);

	// separeteResult
	impl->m_separateResult = iRIC::getBooleanAttribute(node, "separateResult", false);

	// read measured data
	auto tmpNode = iRIC::getChildNode(node, "MeasuredDatas");
	if (! tmpNode.isNull()) {
		impl->loadMeasuredDatas(tmpNode);
	}

	// read calculation result
	tmpNode = iRIC::getChildNode(node, "CalculationResult");
	if (! tmpNode.isNull()) {
		impl->m_postSolutionInfo->loadFromProjectMainFile(tmpNode);
	}

	// read setting about Backgrounds
	tmpNode = iRIC::getChildNode(node, "Backgrounds");
	if (! tmpNode.isNull()) {
		impl->loadBackgrounds(tmpNode);
	}
	m_projectData->mainWindow()->loadSubWindowsFromProjectMainFile(node);
}

void ProjectMainFile::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("version", impl->m_iRICVersion.toString());
	writer.writeAttribute("solverName", impl->m_solverName.c_str());
	writer.writeAttribute("solverVersion", impl->m_solverVersion.toString());

	if (impl->m_coordinateSystem != nullptr) {
		writer.writeAttribute("coordinateSystem", impl->m_coordinateSystem->name());
	}

	if (! impl->m_zeroDateTime.isNull()) {
		writer.writeAttribute("zeroDateTime", impl->m_zeroDateTime.toString(Qt::ISODate));
		writer.writeAttribute("timeZoneOffsetSeconds", QString::number(impl->m_timeZone.standardTimeOffset(impl->m_zeroDateTime)));
	}
	writer.writeAttribute("timeFormat", TimeFormatUtil::toString(impl->m_timeFormat));
	writer.writeAttribute("customTimeFormat", impl->m_customTimeFormat);

	iRIC::setDoubleAttribute(writer, "offsetX", impl->m_offset.x());
	iRIC::setDoubleAttribute(writer, "offsetY", impl->m_offset.y());

	// separeteResult
	iRIC::setBooleanAttribute(writer, "separateResult", impl->m_separateResult);

	// write cgns file list
	writer.writeStartElement("CgnsFileList");
	writer.writeAttribute("current", "Case1");
	writer.writeStartElement("CgnsFileEntry");
	writer.writeAttribute("filename", "Case1");
	writer.writeAttribute("comment", "");
	writer.writeEndElement();
	writer.writeEndElement();

	// write measured data
	writer.writeStartElement("MeasuredDatas");
	impl->saveMeasuredDatas(writer);
	writer.writeEndElement();

	// write setting about CalculationResult
	writer.writeStartElement("CalculationResult");
	impl->m_postSolutionInfo->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write setting about PostProcessors
	writer.writeStartElement("PostProcessors");
	impl->m_postProcessors->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write setting about Backgrounds
	writer.writeStartElement("Backgrounds");
	impl->saveBackgrounds(writer);
	writer.writeEndElement();

	m_projectData->mainWindow()->saveSubWindowsToProjectMainFile(writer);
}

ProjectData* ProjectMainFile::projectData() const
{
	return m_projectData;
}

QString ProjectMainFile::relativeSubPath() const
{
	return "";
}

QStringList ProjectMainFile::containedFiles() const
{
	QStringList ret;
	// Add files those exists in the work folder.
	QDir workDir(projectData()->workDirectory());
	QStringList files = workDir.entryList(QStringList(), QDir::Files, QDir::Name);
	for (int i = 0; i < files.size(); ++i) {
		QString filename = files.at(i);
		if (filename == ProjectData::LOCKFILENAME) {continue;}
		ret << filename;
	}
	// Add CGNS files.
	ret << impl->m_cgnsManager->containedFiles();

	// Add External files in MainWindow
	ret << m_projectData->mainWindow()->containedFiles();

	// Add Background image files
	ret << impl->backgroundImageFiles();

	// Post processor file
	ret << impl->m_postProcessors->containedFiles();

	return ret;
}

bool ProjectMainFile::importCgnsFile(const QString& fname, const QString& newname)
{
	QString to = impl->m_cgnsManager->mainFileFullName().c_str();

	copyCgnsFile(fname, to);

	std::string solverName;
	VersionNumber versionNumber;

	int ret = ProjectCgnsFile::readSolverInfo(to, &solverName, &versionNumber);
	if (ret == IRIC_NO_ERROR) {
		if (impl->m_solverName != solverName || (! impl->m_solverVersion.compatibleWith(versionNumber))) {
			projectData()->setPostOnlyMode();
		}
	} else {
		// error occured reading solver information.
		projectData()->setPostOnlyMode();
	}
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());

	loadFromCgnsFile();

	// CGNS file import is not undo-able.
	iRICUndoStack::instance().clear();
	return true;
}

void ProjectMainFile::exportCurrentCgnsFile()
{
	QString fname = QFileDialog::getSaveFileName(
										m_projectData->mainWindow(), tr("Export CGNS file"), LastIODirectory::get(), tr("CGNS file (*.cgns)")
									);
	if (fname == "") {return;}
	if (QFile::exists(fname)) {
		// if the specified file already exests, remove it first.
		if (! QFile::remove(fname)) {
			QMessageBox::warning(m_projectData->mainWindow(), tr("Error"), tr("Exporting CGNS file failed. Specified file is locked by other process."));
			return;
		}
	}
	// save first.
	saveToCgnsFile();
	// copy to the specified file.
	QFile::copy(currentCgnsFileName(), fname);
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

QString ProjectMainFile::currentCgnsFileName() const
{
	return m_projectData->currentCgnsFileName();
}

int ProjectMainFile::loadFromCgnsFile()
{
	bool rebuildNeeded = false;
	try {
		auto fname = impl->m_cgnsManager->mainFileFullName();
		iRICLib::H5CgnsFile cgnsFile(fname, iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		ValueChangerT<iRICLib::H5CgnsFile*> fileChanger(&(impl->m_cgnsFile), &cgnsFile);
		int ier = m_projectData->mainWindow()->loadFromCgnsFile();
		if (ier != IRIC_NO_ERROR) {return ier;}
	} catch (...) {
		if (impl->m_cgnsManager->backupFileExists()) {
			QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening %1. iRIC tries to salvage data from %2.").arg("Case1.cgn", "Case1_input.cgn"));
			// copy backup file.
			QFile::remove(impl->m_cgnsManager->mainFileFullName().c_str());
			QFile::copy(impl->m_cgnsManager->backupFileFullName().c_str(), impl->m_cgnsManager->mainFileFullName().c_str());
			try {
				auto fname = impl->m_cgnsManager->mainFileFullName();
				iRICLib::H5CgnsFile cgnsFile(fname, iRICLib::H5CgnsFile::Mode::OpenReadOnly);
				ValueChangerT<iRICLib::H5CgnsFile*> fileChanger(&(impl->m_cgnsFile), &cgnsFile);
				int ier = m_projectData->mainWindow()->loadFromCgnsFile();
				if (ier != IRIC_NO_ERROR) {return ier;}
				rebuildNeeded = true;
			}  catch (...) {
				QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening %1.").arg("Case1_input.cgn"));
				return IRIC_H5_CALL_ERROR;
			}
		} else {
			QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening %1.").arg("Case1.cgn"));
			return IRIC_H5_CALL_ERROR;
		}
	}

	postSolutionInfo()->loadFromCgnsFile();
	if (rebuildNeeded) {
		postSolutionInfo()->close();

		int steps = postSolutionInfo()->stepCount();
		iRICLib::H5CgnsFileSeparateSolutionUtil::rebuildBaseIterativeData(impl->m_cgnsManager->mainFileFullName(), steps);

		postSolutionInfo()->loadFromCgnsFile();
	}

	return IRIC_NO_ERROR;
}

int ProjectMainFile::saveToCgnsFile()
{
	// close CGNS file when the solution opened it.
	impl->m_postSolutionInfo->close();

	try {
		auto fname = impl->m_cgnsManager->mainFileFullName();
		iRICLib::H5CgnsFile cgnsFile(fname, iRICLib::H5CgnsFile::Mode::Create);
		ValueChangerT<iRICLib::H5CgnsFile*> fileChanger(&(impl->m_cgnsFile), &cgnsFile);

		int ier = ProjectCgnsFile::writeSolverInfo(&cgnsFile, m_projectData->solverDefinition()->abstract());
		if (ier != IRIC_NO_ERROR) {return ier;}

		return m_projectData->mainWindow()->saveToCgnsFile();
	} catch (...) {
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening CGNS file in project file : Case1.cgn"));
		return IRIC_H5_OPEN_FAIL;
	}
}

int ProjectMainFile::updateCgnsFileOtherThanGrids()
{
	impl->m_postSolutionInfo->close();

	try {
		auto fname = impl->m_cgnsManager->mainFileFullName();
		iRICLib::H5CgnsFile cgnsFile(fname, iRICLib::H5CgnsFile::Mode::OpenModify);
		ValueChangerT<iRICLib::H5CgnsFile*> fileChanger(&(impl->m_cgnsFile), &cgnsFile);
		return m_projectData->mainWindow()->updateCgnsFileOtherThanGrids();
	} catch (...) {
		QMessageBox::critical(m_projectData->mainWindow(), tr("Error"), tr("Error occured while opening CGNS file in project file : Case1.cgn"));
		return IRIC_H5_OPEN_FAIL;
	}
}

bool ProjectMainFile::isModified() const
{
	return impl->m_isModified;
}

ProjectPostProcessors* ProjectMainFile::postProcessors() const
{
	return impl->m_postProcessors;
}

void ProjectMainFile::toggleGridEditFlag()
{
	m_projectData->mainWindow()->toggleGridEditFlag();
}

void ProjectMainFile::closeCgnsFile()
{
	m_projectData->mainWindow()->closeCgnsFile();
	impl->m_postSolutionInfo->closeCgnsFile();
	impl->m_postSolutionInfo->close();
}

const std::vector<BackgroundImageInfo*>& ProjectMainFile::backgroundImages() const
{
	return impl->m_backgroundImages;
}

const std::vector<MeasuredData*>& ProjectMainFile::measuredDatas() const
{
	return impl->m_measuredDatas;
}

const std::vector<vtkRenderer*>& ProjectMainFile::renderers() const
{
	return m_renderers;
}

void ProjectMainFile::clearResults()
{
	impl->m_postSolutionInfo->close();
	impl->m_cgnsManager->deleteResultFolder();

	QFile biFile(PostBaseIterativeValuesContainer::filename(workDirectory()));
	if (biFile.exists()) {biFile.remove();}

	int ier = saveToCgnsFile();
	if (ier != IRIC_NO_ERROR) {return;}

	impl->m_postSolutionInfo->checkCgnsStepsUpdate();
}

bool ProjectMainFile::separateResult() const
{
	return impl->m_separateResult;
}

void ProjectMainFile::setSeparateResult(bool separate)
{
	impl->m_separateResult = separate;
}

bool ProjectMainFile::separateResultExists() const
{
	return impl->m_cgnsManager->separateResultExists();
}

ProjectCgnsManager* ProjectMainFile::cgnsManager() const
{
	return impl->m_cgnsManager;
}

PostSolutionInfo* ProjectMainFile::postSolutionInfo() const
{
	return impl->m_postSolutionInfo;
}

bool ProjectMainFile::hasResults()
{
	return impl->m_postSolutionInfo->hasResults();
}

void ProjectMainFile::addBackgroundImage()
{
	QString dir = LastIODirectory::get();
	QString filter(tr("All images(*.jpg *.jpeg *.png *.tif);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png);;TIFF images(*.tif)"));
	auto mdiArea = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QString fname = QFileDialog::getOpenFileName(mdiArea->currentSubWindow(), tr("Open Image file"), dir, filter);
	if (fname == "") {return;}

	QFileInfo finfo(fname);
	// check whether a image file with the same filename exists.
	QString filename = finfo.fileName();
	for (auto image : impl->m_backgroundImages) {
		if (image->fileName().toLower() == filename.toLower()) {
			// file with the same name already exists.
			QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("A background image with the same name already exists."));
			return;
		}
	}

	// copy to the project file.
	if (! mkdirBGDIR()) {
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("The background image was not added. Please try again."));
		return;
	}

	QDir bgDir = QDir(projectData()->workDirectory());
	bgDir.cd(BGDIR);

	QString to = bgDir.absoluteFilePath(QFileInfo(fname).fileName());
	if (!bgDir.exists(QFileInfo(fname).fileName())){
		if (! QFile::copy(fname, to)) {
			QMessageBox::critical(iricMainWindow(), tr("Warning"),
														tr("Copying image %1 to %2 failed.")
														.arg(QDir::toNativeSeparators(fname))
														.arg(QDir::toNativeSeparators(to)));
			return;
		}
	}

	QString ext = finfo.suffix().toLower();
	if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "tif") {
		try {
			BackgroundImageInfo* image = new BackgroundImageInfo(to, this);
			image->initializePosition(fname);
			addBackgroundImage(image);
		} catch (ErrorMessage m) {
			QMessageBox::warning(iricMainWindow(), tr("Warning"), m);
		}
	} else {
		QMessageBox::warning(iricMainWindow(), tr("Warning"), tr("Invalid image file is specified."));
	}
}

void ProjectMainFile::addBackgroundImage(BackgroundImageInfo* image)
{
	impl->m_backgroundImages.insert(impl->m_backgroundImages.begin(), image);
	emit backgroundImageAdded();
	setModified();
}

bool ProjectMainFile::mkdirBGDIR()
{
	QDir wkDir = QDir(projectData()->workDirectory());
	if (wkDir.cd(BGDIR)) {
		// It already exists.
		return true;
	}

	// create directory.
	wkDir.mkdir(BGDIR);
	if (! wkDir.cd(BGDIR)) {
		return false;
	}
	return true;
}

void ProjectMainFile::deleteImage(const QModelIndex& index)
{
	auto it = impl->m_backgroundImages.begin();
	BackgroundImageInfo* image = *(it + index.row());
	impl->m_backgroundImages.erase(it + index.row());
	image->deleteImageFile();
	delete image;
	emit backgroundImageDeleted(index.row());

	// removing image is not undo-able.
	iRICUndoStack::instance().clear();
	setModified();
}

void ProjectMainFile::moveUpImage(const QModelIndex& index)
{
	if (index.row() == 0) {
		// Can not move up
		return;
	}
	BackgroundImageInfo* i1 = impl->m_backgroundImages.at(index.row());
	BackgroundImageInfo* i2 = impl->m_backgroundImages.at(index.row() - 1);
	impl->m_backgroundImages[index.row()] = i2;
	impl->m_backgroundImages[index.row() - 1] = i1;

	emit backgroundImageMovedUp(index.row());
	setModified();
}

void ProjectMainFile::moveDownImage(const QModelIndex& index)
{
	if (index.row() == impl->m_backgroundImages.size() - 1) {
		// Can not move down
		return;
	}
	BackgroundImageInfo* i1 = impl->m_backgroundImages.at(index.row());
	BackgroundImageInfo* i2 = impl->m_backgroundImages.at(index.row() + 1);
	impl->m_backgroundImages[index.row()] = i2;
	impl->m_backgroundImages[index.row() + 1] = i1;

	emit backgroundImageMovedDown(index.row());
	setModified();
}

void ProjectMainFile::deleteMeasuredData(const QModelIndex& index)
{
	auto it = impl->m_measuredDatas.begin();
	MeasuredData* md = *(it + index.row());
	impl->m_measuredDatas.erase(it + index.row());
	delete md;
	emit measuredDataDeleted(index.row());

	// removing image is not undo-able.
	iRICUndoStack::instance().clear();
	setModified();
}

void ProjectMainFile::moveUpMeasuredData(const QModelIndex& index)
{
	if (index.row() == 0) {
		// Can not move up
		return;
	}
	int row = index.row();
	MeasuredData* data = impl->m_measuredDatas.at(row);

	impl->m_measuredDatas.erase(impl->m_measuredDatas.begin() + row);
	impl->m_measuredDatas.insert(impl->m_measuredDatas.begin() + row - 1, data);

	emit measuredDataMovedUp(index.row());
	setModified();
}

void ProjectMainFile::moveDownMeasuredData(const QModelIndex& index)
{
	if (index.row() == impl->m_measuredDatas.size() - 1) {
		// Can not move down
		return;
	}
	int row = index.row();
	MeasuredData* data = impl->m_measuredDatas.at(row);

	impl->m_measuredDatas.erase(impl->m_measuredDatas.begin() + row);
	impl->m_measuredDatas.insert(impl->m_measuredDatas.begin() + row + 1, data);

	emit measuredDataMovedDown(row);
	setModified();
}

void ProjectMainFile::addRenderer(vtkRenderer* ren)
{
	for (vtkRenderer* r : m_renderers) {
		if (r == ren) {return;}
	}
	m_renderers.push_back(ren);
}

void ProjectMainFile::clearModified()
{
	impl->m_isModified = false;
}

void ProjectMainFile::removeRenderer(vtkRenderer* ren)
{
	for (auto it = m_renderers.begin(); it != m_renderers.end(); ++it) {
		if (*it == ren) {
			m_renderers.erase(it);
			return;
		}
	}
}

void ProjectMainFile::checkVersionCompatibility()
{
	VersionNumber iRICVer = iricMainWindow()->versionNumber();
	VersionNumber projVer = impl->m_iRICVersion;
	if (iRICVer.compatibleWith(projVer)) {
		// OK, no problem.
		return;
	}
	// check for exceptional cases.

	// if iRIC ver. is 1.9 and project ver. is 2.0, it is accepted.
	if (iRICVer.major() == 1 && iRICVer.minor() == 9 && projVer.major() == 2 && projVer.minor() == 0) {
		return;
	}
	// if iRIC ver. is 2.0 and project ver. is 1.9, it is accepted.
	if (iRICVer.major() == 2 && iRICVer.minor() == 0 && projVer.major() == 1 && projVer.minor() == 9) {
		return;
	}

	// error.
	if (iRICVer.major() < projVer.major()) {
		throw ErrorMessage(tr("This project file cannot be read, because it was created by newer iRIC (version %1).").arg(projVer.toString()));
	}
}

void ProjectMainFile::addMeasuredData()
{
	QString dir = LastIODirectory::get();
	QString filter(tr("Text Files (*.csv *.txt);;All Files (*.*)"));
	QString fname = QFileDialog::getOpenFileName(iricMainWindow(), tr("Open Measured Data File"), dir, filter);
	if (fname == "") {return;}

	QFileInfo finfo(fname);
	try {
		MeasuredDataCsvImporter importer;
		MeasuredData* md = importer.importData(fname, offset(), this);
		impl->m_measuredDatas.push_back(md);
		emit measuredDataAdded();
		setModified();
		LastIODirectory::set(finfo.absolutePath());
	} catch (ErrorMessage& message) {
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
	if (! tmpNode.isNull()) {
		impl->m_postProcessors->loadFromProjectMainFile(tmpNode);
	}
}

void ProjectMainFile::loadDom(QDomDocument& doc)
{
	QString fname = filename();
	QFile f(fname);
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(fname).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
}

bool ProjectMainFile::importVisGraphSetting(const QString filename)
{
	QFile f(filename);

	QFileInfo fInfo(filename);
	QDir workDir = fInfo.dir();

	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("Error occured while loading %1.").arg(filename));
		return false;
	}
	QDomElement elem = doc.documentElement();
	std::string solvername = iRIC::toStr(elem.attribute("solverName"));
	QString solverVersion = elem.attribute("solverVersion");
	VersionNumber vn(solverVersion);

	impl->m_postProcessors->loadFromXmlFile(doc.documentElement(), workDir);

	return true;
}

bool ProjectMainFile::exportVisGraphSetting(const QString filename)
{
	if (impl->m_postProcessors->windowCount() == 0) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("There is no visualization/graph windows."));
		return false;
	}
	QFile f(filename);

	QFileInfo fInfo(filename);
	QDir workDir = fInfo.dir();

	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {
		// Failed opening the file.
		QMessageBox::critical(iricMainWindow(), tr("Error"), tr("File %1 could not be opened.").arg(filename));
		return false;
	}

	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	w.writeStartDocument("1.0");
	w.writeStartElement("iRICPostProcessingSettings");
	w.writeAttribute("solverName", impl->m_solverName.c_str());
	w.writeAttribute("solverVersion", impl->m_solverVersion.toString());
	impl->m_postProcessors->saveToXmlFile(w, workDir);
	w.writeEndElement();
	w.writeEndDocument();
	f.close();

	return true;
}

CoordinateSystem* ProjectMainFile::coordinateSystem() const
{
	return impl->m_coordinateSystem;
}

void ProjectMainFile::setCoordinateSystem(CoordinateSystem* system)
{
	if (impl->m_coordinateSystem != nullptr) {
		impl->m_coordinateSystem->free();
	}
	impl->m_coordinateSystem = system;
	if (impl->m_coordinateSystem != nullptr) {impl->m_coordinateSystem->init();}
}

const QDateTime& ProjectMainFile::zeroDateTime() const
{
	return impl->m_zeroDateTime;
}

void ProjectMainFile::setZeroDateTime(const QDateTime& dt)
{
	impl->m_zeroDateTime = dt;
}

const QTimeZone& ProjectMainFile::timeZone() const
{
	return impl->m_timeZone;
}

void ProjectMainFile::setTimeZone(const QTimeZone& timeZone)
{
	impl->m_timeZone = timeZone;
}

TimeFormat ProjectMainFile::timeFormat() const
{
	return impl->m_timeFormat;
}

void ProjectMainFile::setTimeFormat(TimeFormat format)
{
	impl->m_timeFormat = format;
}

QString ProjectMainFile::customTimeFormat() const
{
	return impl->m_customTimeFormat;
}

void ProjectMainFile::setCustomTimeFormat(const QString& format)
{
	impl->m_customTimeFormat = format;
}

bool ProjectMainFile::showTimeZone() const
{
	return impl->m_showTimeZone;
}

void ProjectMainFile::setShowTimeZone(bool show)
{
	impl->m_showTimeZone = show;
}

QPointF ProjectMainFile::offset() const
{
	return impl->m_offset;
}

void ProjectMainFile::setOffset(double x, double y)
{
	double x_diff = x - impl->m_offset.x();
	double y_diff = y - impl->m_offset.y();

	impl->m_offset.setX(x);
	impl->m_offset.setY(y);

	projectData()->mainWindow()->preProcessorWindow()->dataModel()->applyOffset(x_diff, y_diff);
	projectData()->mainfile()->postSolutionInfo()->applyOffset(x_diff, y_diff);
	impl->m_postProcessors->applyOffset(x_diff, y_diff);
}

iRICLib::H5CgnsFile* ProjectMainFile::cgnsFile() const
{
	return impl->m_cgnsFile;
}

class ProjectSetOffsetCommand : public QUndoCommand
{
public:
	ProjectSetOffsetCommand(const QPointF& newoffset, ProjectMainFile* file) :
		QUndoCommand(QObject::tr("Set offset"))
	{
		m_newOffset = newoffset;
		m_oldOffset = file->offset();
		m_projectMainFile = file;
	}
	void redo() {
		m_projectMainFile->setOffset(m_newOffset.x(), m_newOffset.y());
	}
	void undo() {
		m_projectMainFile->setOffset(m_oldOffset.x(), m_oldOffset.y());
	}

private:
	QPointF m_newOffset;
	QPointF m_oldOffset;
	ProjectMainFile* m_projectMainFile;
};

void ProjectMainFile::setupOffset()
{
	OffsetSettingDialog dialog;
	dialog.setOffset(impl->m_offset.x(), impl->m_offset.y());
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	iRICUndoStack::instance().push(new ProjectSetOffsetCommand(QPointF(dialog.offsetX(), dialog.offsetY()), this));
}

int ProjectMainFile::showCoordinateSystemDialog(bool forceSelect)
{
	auto mainW = projectData()->mainWindow();
	CoordinateSystemSelectDialog dialog(mainW);
	dialog.setBuilder(mainW->coordinateSystemBuilder());
	dialog.setCoordinateSystem(coordinateSystem());

	CoordinateSystem* cs = nullptr;
	do {
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {return ret;}
		cs = dialog.coordinateSystem();

		if (cs == nullptr && forceSelect) {
			QMessageBox::warning(mainW, tr("Warning"), tr("Coordinate system not selected."));
		}
	} while (cs == nullptr && forceSelect);

	setCoordinateSystem(cs);
	return QDialog::Accepted;
}
