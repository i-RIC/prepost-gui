#include "colortransferfunctioncontainer.h"

#include <guibase/colortool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>

ColorTransferFunctionContainer::ColorTransferFunctionContainer(ProjectDataItem* d) :
	ScalarsToColorsContainer {d}
{
	setVtkObj(vtkColorTransferFunction::New());
	setVtkDarkObj(vtkColorTransferFunction::New());
}

void ColorTransferFunctionContainer::update()
{
	vtkColorTransferFunction* ctf = dynamic_cast<vtkColorTransferFunction*>(vtkObj());
	vtkColorTransferFunction* ctfDark = dynamic_cast<vtkColorTransferFunction*>(vtkDarkObj());

	ctf->RemoveAllPoints();
	ctfDark->RemoveAllPoints();
	double c[3];
	for (auto it = m_colors.begin(); it != m_colors.end(); ++it) {
		iRIC::QColorToVTKColor(it.value(), c);
		ctf->AddRGBPoint(it.key(), c[0], c[1], c[2]);
		ctfDark->AddRGBPoint(it.key(), c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
	}
}

const QMap<double, QColor> ColorTransferFunctionContainer::colors() const
{
	return m_colors;
}

void ColorTransferFunctionContainer::setColors(const QMap<double, QColor>& map)
{
	m_colors = map;
	update();
}

const QMap<double, QString>& ColorTransferFunctionContainer::enumerations() const
{
	return m_enumerations;
}

void ColorTransferFunctionContainer::setEnumerations(const QMap<double, QString>& map)
{
	m_enumerations = map;
}

const QMap<double, QString>& ColorTransferFunctionContainer::englishEnumerations() const
{
	return m_englishEnumerations;
}

void ColorTransferFunctionContainer::setEnglishEnumerations(const QMap<double, QString>& map)
{
	m_englishEnumerations = map;
}

void ColorTransferFunctionContainer::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (int i = 0; i < node.childNodes().count(); ++i){
		QDomElement elem = node.childNodes().at(i).toElement();
		double val = iRIC::getDoubleAttribute(elem, "value");
		if (m_colors.contains(val)) {
			m_colors[val] = iRIC::getColorAttribute(elem, "color");
		}
	}
	update();
}

void ColorTransferFunctionContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_colors.begin(); it != m_colors.end(); ++it){
		writer.writeStartElement("Color");
		iRIC::setDoubleAttribute(writer, "value", it.key());
		iRIC::setColorAttribute(writer, "color", it.value());
		writer.writeEndElement();
	}
}
