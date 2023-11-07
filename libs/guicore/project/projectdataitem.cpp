#include "../base/iricmainwindowinterface.h"
#include "../base/windowwithzindexinterface.h"
#include "projectdata.h"
#include "projectdataitem.h"
#include "projectmainfile.h"
#include "private/projectdataitem_impl.h"

#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomElement>
#include <QRect>
#include <QString>
#include <QWidget>
#include <QXmlStreamWriter>

#include <vtkCamera.h>

#include <iriclib_errorcodes.h>

// Public interfaces

const QDataStream::Version ProjectDataItem::dataStreamVersion = QDataStream::Qt_4_6;

ProjectDataItem::ProjectDataItem(ProjectDataItem* d) :
	ProjectDataItem {"", d}
{}

ProjectDataItem::ProjectDataItem(const QString& filename, ProjectDataItem* d) :
	QObject {d},
	impl {new Impl {}}
{
	impl->m_filename = filename;
}

ProjectDataItem::~ProjectDataItem()
{
	delete impl;
}

void ProjectDataItem::loadFromProjectMainFile(const QDomNode& node)
{
	// load data from project main file.
	doLoadFromProjectMainFile(node);
	// when this item stores data in external file, load it.
	if (impl->m_filename != "") {
		loadExternalData(filename());
	}
}

void ProjectDataItem::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	// when this item defines a sub folder, create the subfolder.
	if (impl->m_subPath != "") {
		QString parentFolder = QDir(projectData()->workDirectory()).absoluteFilePath(parent()->relativeSubPath());
		QDir(parentFolder).mkdir(impl->m_subPath);
	}
	// save data to project main file.
	doSaveToProjectMainFile(writer);
	// when this item stores data in external file, save it.
	if (impl->m_filename != "") {
		saveExternalData(filename());
	}
}

QString ProjectDataItem::currentCgnsFileName() const
{
	return parent()->currentCgnsFileName();
}

int ProjectDataItem::loadFromCgnsFile()
{
	return IRIC_NO_ERROR;
}

int ProjectDataItem::saveToCgnsFile()
{
	return IRIC_NO_ERROR;
}

int ProjectDataItem::updateCgnsFileOtherThanGrids()
{
	return IRIC_NO_ERROR;
}

void ProjectDataItem::closeCgnsFile()
{}

QString ProjectDataItem::filename() const
{
	if (impl->m_filename == "") {return "";}

	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeFilename());
}

QString ProjectDataItem::relativeFilename() const
{
	QDir subdir(relativeSubPath());
	return subdir.filePath(impl->m_filename);
}

void ProjectDataItem::setFilename(const QString& fname)
{
	impl->m_filename = fname;
}

QStringList ProjectDataItem::containedFiles() const
{
	QStringList ret;
	if (impl->m_filename != "") {
		ret.append(relativeFilename());
	}
	return ret;
}

ProjectDataItem* ProjectDataItem::parent() const
{
	return dynamic_cast<ProjectDataItem*> (QObject::parent());
}

iRICMainWindowInterface* ProjectDataItem::iricMainWindow() const
{
	auto p = projectData();
	if (p == nullptr) {return nullptr;}

	return p->mainWindow();
}

void ProjectDataItem::setModified(bool modified)
{
	ProjectDataItem* p = parent();
	if (p == nullptr) {return;}
	p->setModified(modified);
}

ProjectData* ProjectDataItem::projectData() const
{
	auto p = parent();
	if (p == nullptr) {return nullptr;}
	return p->projectData();
}

void ProjectDataItem::loadFilename(const QDomNode& node)
{
	impl->m_filename = node.toElement().attribute("filename");
}

void ProjectDataItem::saveFilename(QXmlStreamWriter& writer) const
{
	if (impl->m_filename.isNull()) {return;}

	writer.writeAttribute("filename", impl->m_filename);
}

QString ProjectDataItem::relativeSubPath() const
{
	if (impl->m_subPath == "") {
		return parent()->relativeSubPath();
	}

	QString parentPath = parent()->relativeSubPath();
	if (parentPath != "") {parentPath.append("/");}
	return parentPath.append(impl->m_subPath);
}

QString ProjectDataItem::subPath() const
{
	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeSubPath());
}

void ProjectDataItem::setSubPath(const QString& subPath)
{
	impl->m_subPath = subPath;
}

QPointF ProjectDataItem::offset() const
{
	auto pd = projectData();
	if (pd == nullptr) {return QPointF(0, 0);}
	return pd->mainfile()->offset();
}

void ProjectDataItem::doApplyOffset(double, double)
{}

void ProjectDataItem::loadExternalData(const QString&)
{}

void ProjectDataItem::saveExternalData(const QString&)
{}

QColor ProjectDataItem::loadColorAttribute(const QString& name, const QDomNode& node, const QColor& defaultColor)
{
	QString colval = node.toElement().attribute(name, defaultColor.name());
	return QColor(colval);
}

void ProjectDataItem::writeColorAttribute(const QString& name, const QColor& col, QXmlStreamWriter& writer)
{
	writer.writeAttribute(name, col.name());
}

QColor ProjectDataItem::loadBackgroundColor(const QDomNode& node, const QColor& defaultColor)
{
	return loadColorAttribute("backgroundColor", node, defaultColor);
}

void ProjectDataItem::writeBackgroundColor(QColor c, QXmlStreamWriter& writer)
{
	writeColorAttribute("backgroundColor", c, writer);
}

void ProjectDataItem::loadCamera(vtkCamera* camera, const QDomNode& node)
{
	double position[3];
	double focalpoint[3];
	double viewup[3];

	// order based on vtkSynchronizedRenderers::RendererInfo::CopyTo()
	// from ParaView v4.4.0
	if (iRIC::getBooleanAttribute(node, "parallelprojection", true)) {
		camera->ParallelProjectionOn();
	} else {
		camera->ParallelProjectionOff();
	}

	position[0] = iRIC::getDoubleAttribute(node, "positionX");
	position[1] = iRIC::getDoubleAttribute(node, "positionY");
	position[2] = iRIC::getDoubleAttribute(node, "positionZ", 200);

	camera->SetPosition(position);

	focalpoint[0] = iRIC::getDoubleAttribute(node, "focalpointX");
	focalpoint[1] = iRIC::getDoubleAttribute(node, "focalpointY");
	focalpoint[2] = iRIC::getDoubleAttribute(node, "focalpointZ");

	camera->SetFocalPoint(focalpoint);

	viewup[0] = iRIC::getDoubleAttribute(node, "viewupX");
	viewup[1] = iRIC::getDoubleAttribute(node, "viewupY");
	viewup[2] = iRIC::getDoubleAttribute(node, "viewupZ");

	camera->SetViewUp(viewup);

	camera->SetRoll(iRIC::getDoubleAttribute(node, "roll"));
	camera->SetParallelScale(iRIC::getDoubleAttribute(node, "parallelscale", 1));
}

void ProjectDataItem::saveCamera(vtkCamera* camera, QXmlStreamWriter& writer)
{
	double position[3];
	double focalpoint[3];
	double viewup[3];

	camera->GetPosition(position);
	camera->GetFocalPoint(focalpoint);
	camera->GetViewUp(viewup);

	iRIC::setDoubleAttribute(writer, "positionX", position[0]);
	iRIC::setDoubleAttribute(writer, "positionY", position[1]);
	iRIC::setDoubleAttribute(writer, "positionZ", position[2]);

	iRIC::setDoubleAttribute(writer, "focalpointX", focalpoint[0]);
	iRIC::setDoubleAttribute(writer, "focalpointY", focalpoint[1]);
	iRIC::setDoubleAttribute(writer, "focalpointZ", focalpoint[2]);

	iRIC::setDoubleAttribute(writer, "viewupX", viewup[0]);
	iRIC::setDoubleAttribute(writer, "viewupY", viewup[1]);
	iRIC::setDoubleAttribute(writer, "viewupZ", viewup[2]);

	iRIC::setDoubleAttribute(writer, "roll", camera->GetRoll());
	iRIC::setDoubleAttribute(writer, "parallelscale", camera->GetParallelScale());
	if (camera->GetParallelProjection()) {
		iRIC::setBooleanAttribute(writer, "parallelprojection", true);
	} else {
		iRIC::setBooleanAttribute(writer, "parallelprojection", false);
	}
}

void ProjectDataItem::loadExternalData()
{
	loadExternalData(filename());
}
