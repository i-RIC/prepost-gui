#include "../base/iricmainwindowinterface.h"
#include "../base/windowwithzindexinterface.h"
#include "projectdata.h"
#include "projectdataitem.h"
#include "projectmainfile.h"

#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomElement>
#include <QRect>
#include <QString>
#include <QWidget>
#include <QXmlStreamWriter>

#include <vtkCamera.h>

class ProjectDataItem::Impl
{
public:
	QString m_filename {};
	QString m_subPath {};
};

// Public interfaces

const QDataStream::Version ProjectDataItem::dataStreamVersion = QDataStream::Qt_4_6;

ProjectDataItem::ProjectDataItem(ProjectDataItem* d) :
	ProjectDataItem {"", d}
{}

ProjectDataItem::ProjectDataItem(const QString& filename, ProjectDataItem* d) :
	QObject {d},
	m_impl {new Impl {}}
{
	m_impl->m_filename = filename;
}

ProjectDataItem::~ProjectDataItem()
{
	delete m_impl;
}

void ProjectDataItem::loadFromProjectMainFile(const QDomNode& node)
{
	// load data from project main file.
	doLoadFromProjectMainFile(node);
	// when this item stores data in external file, load it.
	if (m_impl->m_filename != "") {
		loadExternalData(filename());
	}
}

void ProjectDataItem::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	// when this item defines a sub folder, create the subfolder.
	if (m_impl->m_subPath != "") {
		QString parentFolder = QDir(projectData()->workDirectory()).absoluteFilePath(parent()->relativeSubPath());
		QDir(parentFolder).mkdir(m_impl->m_subPath);
	}
	// save data to project main file.
	doSaveToProjectMainFile(writer);
	// when this item stores data in external file, save it.
	if (m_impl->m_filename != "") {
		saveExternalData(filename());
	}
}

QString ProjectDataItem::currentCgnsFileName() const
{
	return parent()->currentCgnsFileName();
}

void ProjectDataItem::loadFromCgnsFile(int)
{}

void ProjectDataItem::saveToCgnsFile(int)
{}

void ProjectDataItem::closeCgnsFile()
{}

QString ProjectDataItem::filename() const
{
	if (m_impl->m_filename == "") {return "";}

	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeFilename());
}

QString ProjectDataItem::relativeFilename() const
{
	QDir subdir(relativeSubPath());
	return subdir.filePath(m_impl->m_filename);
}

void ProjectDataItem::setFilename(const QString& fname)
{
	m_impl->m_filename = fname;
}

QStringList ProjectDataItem::containedFiles()
{
	QStringList ret;
	if (m_impl->m_filename != "") {
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
	return projectData()->mainWindow();
}

void ProjectDataItem::setModified(bool modified)
{
	ProjectDataItem* p = parent();
	if (p == nullptr) {return;}
	p->setModified(modified);
}

ProjectData* ProjectDataItem::projectData() const
{
	return parent()->projectData();
}

void ProjectDataItem::loadFilename(const QDomNode& node)
{
	m_impl->m_filename = node.toElement().attribute("filename");
}

void ProjectDataItem::saveFilename(QXmlStreamWriter& writer) const
{
	if (m_impl->m_filename.isNull()) {return;}

	writer.writeAttribute("filename", m_impl->m_filename);
}

QString ProjectDataItem::relativeSubPath() const
{
	if (m_impl->m_subPath == "") {
		return parent()->relativeSubPath();
	}

	QString parentPath = parent()->relativeSubPath();
	if (parentPath != "") {parentPath.append("/");}
	return parentPath.append(m_impl->m_subPath);
}

QString ProjectDataItem::subPath() const
{
	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeSubPath());
}

void ProjectDataItem::setSubPath(const QString& subPath)
{
	m_impl->m_subPath = subPath;
}

QVector2D ProjectDataItem::offset() const
{
	return projectData()->mainfile()->offset();
}

void ProjectDataItem::doApplyOffset(double, double)
{}

void ProjectDataItem::loadExternalData(const QString&)
{}

void ProjectDataItem::saveExternalData(const QString&)
{}

void ProjectDataItem::loadWindowGeometry(QWidget* w, const QDomNode& node)
{
	QDomElement elem = node.toElement();
	QPoint pos;
	pos.setX(elem.attribute("left").toInt());
	pos.setY(elem.attribute("top").toInt());
	w->move(pos);
	QSize size;
	size.setWidth(elem.attribute("width").toInt());
	size.setHeight(elem.attribute("height").toInt());
	w->resize(size);
	bool visible = elem.attribute("visible").toInt();
	w->setVisible(visible);

	bool max = elem.attribute("maximized").toInt();
	if (max) {
		w->showMaximized();
	}
}

void ProjectDataItem::writeWindowGeometry(QWidget* w, QXmlStreamWriter& writer)
{
	QString tmp;
	// visibility
	tmp.setNum(w->isVisible());
	writer.writeAttribute("visible", tmp);
	// maximized
	tmp.setNum(w->isMaximized());
	writer.writeAttribute("maximized", tmp);
	// rect
	QRect rect = w->frameGeometry();
	tmp.setNum(rect.left());
	writer.writeAttribute("left", tmp);
	tmp.setNum(rect.top());
	writer.writeAttribute("top", tmp);
	tmp.setNum(rect.width());
	writer.writeAttribute("width", tmp);
	tmp.setNum(rect.height());
	writer.writeAttribute("height", tmp);
}

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

int ProjectDataItem::loadOpacityPercent(const QDomNode& node)
{
	return iRIC::getIntAttribute(node, "opacityPercent", 100);
}

void ProjectDataItem::writeOpacityPercent(int opacity, QXmlStreamWriter& writer)
{
	iRIC::setIntAttribute(writer, "opacityPercent", opacity);
}

void ProjectDataItem::loadCamera(vtkCamera* camera, const QDomNode& node)
{
	double position[3];
	double focalpoint[3];

	position[0] = iRIC::getDoubleAttribute(node, "positionX");
	position[1] = iRIC::getDoubleAttribute(node, "positionY");
	position[2] = iRIC::getDoubleAttribute(node, "positionZ", 200);

	camera->SetPosition(position);

	focalpoint[0] = iRIC::getDoubleAttribute(node, "focalpointX");
	focalpoint[1] = iRIC::getDoubleAttribute(node, "focalpointY");
	focalpoint[2] = iRIC::getDoubleAttribute(node, "focalpointZ");

	camera->SetFocalPoint(focalpoint);

	camera->SetRoll(iRIC::getDoubleAttribute(node, "roll"));
	camera->SetParallelScale(iRIC::getDoubleAttribute(node, "parallelscale", 1));
}

void ProjectDataItem::saveCamera(vtkCamera* camera, QXmlStreamWriter& writer)
{
	double position[3];
	double focalpoint[3];

	camera->GetPosition(position);
	camera->GetFocalPoint(focalpoint);

	iRIC::setDoubleAttribute(writer, "positionX", position[0]);
	iRIC::setDoubleAttribute(writer, "positionY", position[1]);
	iRIC::setDoubleAttribute(writer, "positionZ", position[2]);

	iRIC::setDoubleAttribute(writer, "focalpointX", focalpoint[0]);
	iRIC::setDoubleAttribute(writer, "focalpointY", focalpoint[1]);
	iRIC::setDoubleAttribute(writer, "focalpointZ", focalpoint[2]);

	iRIC::setDoubleAttribute(writer, "roll", camera->GetRoll());
	iRIC::setDoubleAttribute(writer, "parallelscale", camera->GetParallelScale());
}

void ProjectDataItem::loadExternalData()
{
	loadExternalData(filename());
}
