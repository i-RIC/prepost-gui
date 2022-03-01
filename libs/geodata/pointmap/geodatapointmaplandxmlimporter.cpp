#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapt.h"

#include <misc/xmlsupport.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QDebug>
#include <QDomDocument>
#include <QElapsedTimer>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>

namespace {

QDomNode findNode(const QDomElement& elem, const QStringList& nodeNames)
{
	if (nodeNames.size() == 0) {return elem;}
	auto names = nodeNames;
	QString name = names.at(0);
	names.pop_front();

	auto node = iRIC::getChildNode(elem, name);
	if (node.isNull()) {return node;}

	return findNode(node.toElement(), names);
}

bool readUntil(QXmlStreamReader& xml, const char* elemName)
{
	while (! xml.atEnd()) {
		xml.readNextStartElement();
		if (xml.name().toString().compare(elemName) == 0) {
			return true; // success
		}
	}
	return false; // failure
}

bool readUntilAndSkip(QXmlStreamReader& xml, const char* elemName)
{
	while (! xml.atEnd()) {
		xml.readNextStartElement();
		if (xml.name().toString().compare(elemName) == 0) {
			xml.skipCurrentElement();
			return true; // success
		}
	}
	return false; // failure
}

} // namespace

GeoDataPointmapLandXmlImporter::GeoDataPointmapLandXmlImporter(GeoDataCreator* creator) :
	GeoDataImporter("pointmap_landxml", tr("LandXML (*.xml)"), creator)
{}

bool GeoDataPointmapLandXmlImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	{
		// Still using the DOM parser to check for xml errors
		//

		QElapsedTimer timer;
		timer.start();
		QFile f(filename());
		bool ok = f.open(QFile::ReadOnly);

		QDomDocument doc;
		QString errorStr;
		int errorLine, errorColumn;
		ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Parse error %1 at line %2 column %3").arg(errorStr).arg(errorLine).arg(errorColumn));
			return false;
		}
		f.close();
		qDebug() << "precheck DOM" << timer.elapsed() << "milliseconds";    // Floodplain Channels.xml 486 479 491
	}

	int index = 0;
	QElapsedTimer timer;
	timer.start();
	// bool bret = this->importDataDOM(data, index, w);
	// qDebug() << "importDataDOM" << timer.elapsed() << "milliseconds";    // Floodplain Channels.xml -- 44844 44654 44729
	bool ret = this->importDataSAX(data, index, w);
	qDebug() << "importDataSAX" << timer.elapsed() << "milliseconds";    // Floodplain Channels.xml -- 641 636 640
	return ret;
}

bool GeoDataPointmapLandXmlImporter::importDataDOM(GeoData* data, int /*index*/, QWidget* w)
{
	QFile f(filename());
	bool ok = f.open(QFile::ReadOnly);

	QDomDocument doc;
	QString errorStr;
	int errorLine, errorColumn;
	ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (!ok) {
		QMessageBox::critical(w, tr("Error"), tr("Parse error %1 at line %2 column %3").arg(errorStr).arg(errorLine).arg(errorColumn));
		return false;
	}
	f.close();

	auto pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*> (data);
	auto points = vtkSmartPointer<vtkPoints>::New();
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	auto cellArray = vtkSmartPointer<vtkCellArray>::New();
	points->SetDataTypeToDouble();
	auto values = vtkSmartPointer<vtkDoubleArray>::New();
	polyData->SetPoints(points);

	QStringList names;
	names << "Surfaces" << "Surface";
	QDomNode surfaceNode = findNode(doc.documentElement(), names);
	if (surfaceNode.isNull()) { return false; }

	auto defNode = iRIC::getChildNode(surfaceNode, "Definition");
	auto pntsNode = iRIC::getChildNode(defNode, "Pnts");

	std::map<int, int> pointIdMap;

	int numPoints = 0;
	for (int i = 0; i < pntsNode.childNodes().count(); ++i) {
		auto pNode = pntsNode.childNodes().at(i);
		auto pElem = pNode.toElement();
		auto pIdStr = pElem.attribute("id");
		int pointId = numPoints + 1;
		if (!pIdStr.isEmpty()) {
			pointId = pIdStr.toInt();
		}
		QStringList frags = pElem.text().split(" ");
		double y = frags.at(0).toDouble();
		double x = frags.at(1).toDouble();
		double z = frags.at(2).toDouble();

		points->InsertNextPoint(x, y, 0);
		values->InsertNextValue(z);

		pointIdMap.insert({ pointId, numPoints });
		++numPoints;
	}
	auto facesNode = iRIC::getChildNode(defNode, "Faces");
	for (int i = 0; i < facesNode.childNodes().count(); ++i) {
		vtkIdType ids[3];
		auto fNode = facesNode.childNodes().at(i);
		QStringList frags = fNode.toElement().text().split(" ");
		ids[0] = pointIdMap.at(frags.at(0).toInt());
		ids[1] = pointIdMap.at(frags.at(1).toInt());
		ids[2] = pointIdMap.at(frags.at(2).toInt());
		cellArray->InsertNextCell(3, ids);
	}
	polyData->SetPolys(cellArray);
	pmap->setSTLData(polyData, values);
	return true;
}

bool GeoDataPointmapLandXmlImporter::importDataSAX(GeoData* data, int /*index*/, QWidget* /*w*/)
{
	QFile f(filename());
	bool ok = f.open(QFile::ReadOnly);

	auto pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*> (data);
	auto points = vtkSmartPointer<vtkPoints>::New();
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	auto cellArray = vtkSmartPointer<vtkCellArray>::New();
	points->SetDataTypeToDouble();
	auto values = vtkSmartPointer<vtkDoubleArray>::New();
	polyData->SetPoints(points);

	QXmlStreamReader xml;
	xml.setDevice(&f);

	if (! readUntil(xml, "Surfaces")) return false;

	if (! readUntil(xml, "Surface")) return false;

	// Not sure if SourceData element is required
	// 
	// if (! readUntilAndSkip(xml, "SourceData")) return false;

	if (! readUntil(xml, "Definition")) return false;

	if (! readUntil(xml, "Pnts")) return false;

	int numPoints = 0;
	std::map<int, int> pointIdMap;

	while (! xml.atEnd()) {
		xml.readNextStartElement();
		if (xml.name().toString().compare("P") == 0) {

			auto pIdStr = xml.attributes().value("id");

			int pointId = numPoints + 1;
			if (! pIdStr.isEmpty()) {
				pointId = pIdStr.toInt();
			}

			QStringList frags = xml.readElementText().split(" ");
			double y = frags.at(0).toDouble();
			double x = frags.at(1).toDouble();
			double z = frags.at(2).toDouble();

			points->InsertNextPoint(x, y, 0);
			values->InsertNextValue(z);

			pointIdMap.insert({ pointId, numPoints });
			++numPoints;
		} else {
			break;
		}
	}

	if (! readUntil(xml, "Faces")) return false;

	while (! xml.atEnd()) {
		xml.readNextStartElement();
		if (xml.name().toString().compare("F") == 0) {
			vtkIdType ids[3];
			QStringList frags = xml.readElementText().split(" ");

			ids[0] = pointIdMap.at(frags.at(0).toInt());
			ids[1] = pointIdMap.at(frags.at(1).toInt());
			ids[2] = pointIdMap.at(frags.at(2).toInt());
			cellArray->InsertNextCell(3, ids);
		} else {
			break;
		}
	}

	polyData->SetPolys(cellArray);
	pmap->setSTLData(polyData, values);
	return ok;
}

const QStringList GeoDataPointmapLandXmlImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("LandXML File (*.xml)");
	return ret;
}

const QStringList GeoDataPointmapLandXmlImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "xml";
	return ret;
}
