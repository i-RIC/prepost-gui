#include "projectdataitem.h"
#include "projectdata.h"
#include "projectmainfile.h"
#include "../base/iricmainwindowinterface.h"
#include "../base/windowwithzindex.h"
#include <misc/xmlsupport.h>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <QXmlStreamWriter>
#include <QDomElement>
#include <QDir>

#include <vtkCamera.h>

const QString ProjectDataItem::filename()
{
	if (m_filename == ""){return "";}
	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeFilename());
}

const QString ProjectDataItem::relativeFilename()
{
	QDir subdir(relativeSubPath());
	return subdir.filePath(m_filename);
}

QStringList ProjectDataItem::containedFiles()
{
	QStringList ret;
	if (m_filename != ""){ret.append(relativeFilename());}
	return ret;
}

void ProjectDataItem::loadFromProjectMainFile(const QDomNode& node)
{
	// load data from project main file.
	doLoadFromProjectMainFile(node);
	// when this item stores data in external file, load it.
	if (m_filename != ""){
		loadExternalData(filename());
	}
}

void ProjectDataItem::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	// when this item defines a sub folder, create the subfolder.
	if (m_subFolder != ""){
		QDir(QDir(projectData()->workDirectory()).absoluteFilePath(parent()->relativeSubPath())).mkdir(m_subFolder);
	}
	// save data to project main file.
	doSaveToProjectMainFile(writer);
	// when this item stores data in external file, save it.
	if (m_filename != ""){
		saveExternalData(filename());
	}
}

void ProjectDataItem::loadFilename(const QDomNode& node)
{
	m_filename = node.toElement().attribute("filename");
}

void ProjectDataItem::saveFilename(QXmlStreamWriter& writer) const
{
	if (! m_filename.isNull()){
		writer.writeAttribute("filename", m_filename);
	}
}

void ProjectDataItem::loadWindowGeometry(QWidget *w, const QDomNode &node)
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
	if (max){
		w->showMaximized();
	}
//	WindowWithZIndex* w2 = dynamic_cast<WindowWithZIndex*>(w);
//	w2->setZindex(elem.attribute("zindex").toInt());
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

QString ProjectDataItem::relativeSubPath() const
{
	if (m_subFolder != ""){
		QString parentPath = parent()->relativeSubPath();
		if (parentPath != ""){parentPath.append("/");}
		return parentPath.append(m_subFolder);
	}else{
		return parent()->relativeSubPath();
	}
}

QString ProjectDataItem::subPath() const
{
	QDir workdir(projectData()->workDirectory());
	return workdir.absoluteFilePath(relativeSubPath());
}

bool ProjectDataItem::isNear(const QPoint& p1, const QPoint& p2)
{
	int xdiff = p2.x() - p1.x();
	int ydiff = p2.y() - p2.y();
	if (xdiff < - 2 || xdiff > 2){return false;}
	if (ydiff < - 2 || ydiff > 2){return false;}
	return true;
}

iRICMainWindowInterface* ProjectDataItem::iricMainWindow()
{
	return projectData()->mainWindow();
}

void ProjectDataItem::loadCamera(vtkCamera* camera, const QDomNode& node)
{
	double position[3];
	double focalpoint[3];

	QDomElement elem = node.toElement();
	position[0] = elem.attribute("positionX").toDouble();
	position[1] = elem.attribute("positionY").toDouble();
	position[2] = elem.attribute("positionZ").toDouble();

	camera->SetPosition(position);

	focalpoint[0] = elem.attribute("focalpointX").toDouble();
	focalpoint[1] = elem.attribute("focalpointY").toDouble();
	focalpoint[2] = elem.attribute("focalpointZ").toDouble();

	camera->SetFocalPoint(focalpoint);

	camera->SetRoll(elem.attribute("roll").toDouble());
	camera->SetParallelScale(elem.attribute("parallelscale").toDouble());
}

void ProjectDataItem::saveCamera(vtkCamera* camera, QXmlStreamWriter& writer)
{
	double position[3];
	double focalpoint[3];

	camera->GetPosition(position);
	camera->GetFocalPoint(focalpoint);

	writer.writeAttribute("positionX", QString::number(position[0]));
	writer.writeAttribute("positionY", QString::number(position[1]));
	writer.writeAttribute("positionZ", QString::number(position[2]));

	writer.writeAttribute("focalpointX", QString::number(focalpoint[0]));
	writer.writeAttribute("focalpointY", QString::number(focalpoint[1]));
	writer.writeAttribute("focalpointZ", QString::number(focalpoint[2]));

	writer.writeAttribute("roll", QString::number(camera->GetRoll()));
	writer.writeAttribute("parallelscale", QString::number(camera->GetParallelScale()));
}

void ProjectDataItem::loadExternalData()
{
	loadExternalData(filename());
}

QVector2D ProjectDataItem::offset()
{
	return projectData()->mainfile()->offset();
}
