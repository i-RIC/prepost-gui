#include "landxmlutil.h"

#include <cs/coordinatesystem.h>

#include <QDateTime>
#include <QXmlStreamWriter>

void LandXmlUtil::writeStartLandXml(QXmlStreamWriter* writer)
{
	QDateTime dt = QDateTime::currentDateTime();

	writer->writeStartElement("LandXML");
	writer->writeAttribute("xmlns", "http://www.landxml.org/schema/LandXML-1.2");
	writer->writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	writer->writeAttribute("xsi:schemaLocation", "http://www.landxml.org/schema/LandXML-1.2 http://www.landxml.org/schema/LandXML-1.2/LandXML-1.2.xsd");
	writer->writeAttribute("date", dt.toString("yyyy-MM-dd"));
	writer->writeAttribute("time", dt.toString("hh:mm:ss"));
	writer->writeAttribute("version", "1.2");
}

void LandXmlUtil::writeProjectForMlit(QXmlStreamWriter* writer, const QString& name)
{
	writer->writeStartElement("Project");
	writer->writeAttribute("name", name);

	writer->writeStartElement("Feature");

	writer->writeStartElement("Property");
	writer->writeAttribute("label", "applicationCriterion");
	writer->writeAttribute("value", "MlitLandXmlVer.1.2");
	writer->writeEndElement(); // Property

	writer->writeEndElement(); // Feature

	writer->writeEndElement(); // Project
}

void LandXmlUtil::writeApplication(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Application");
	writer->writeAttribute("name", "iRIC");
	writer->writeEndElement(); // Application
}

void LandXmlUtil::writeUnits(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Units");
	writer->writeStartElement("Metric");
	writer->writeAttribute("areaUnit", "squareMeter");
	writer->writeAttribute("linearUnit", "meter");
	writer->writeAttribute("volumeUnit", "cubicMeter");
	writer->writeAttribute("directionUnit", "decimal degrees");
	writer->writeEndElement(); // Metric
	writer->writeEndElement(); // Units
}

void LandXmlUtil::writeCoordinateSystem(QXmlStreamWriter* writer, CoordinateSystem* cs)
{
	if (cs == nullptr) {return;}

	writer->writeStartElement("CoordinateSystem");
	auto name = cs->name();
	auto epsgCode = name.replace("EPSG:", "");
	writer->writeAttribute("epsgCode", epsgCode);
	writer->writeAttribute("name", cs->caption());
	writer->writeEndElement(); // CoordinateSystem
}
