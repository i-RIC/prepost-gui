#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapt.h"

#include <misc/xmlsupport.h>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QDomDocument>
#include <QFile>
#include <QMessageBox>

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

} // namespace

GeoDataPointmapLandXmlImporter::GeoDataPointmapLandXmlImporter(GeoDataCreator* creator) :
	GeoDataImporter("pointmap_landxml", tr("LandXML (*.xml)"), creator)
{}

bool GeoDataPointmapLandXmlImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
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
	if (surfaceNode.isNull()) {return false;}

	auto defNode = iRIC::getChildNode(surfaceNode, "Definition");
	auto pntsNode = iRIC::getChildNode(defNode, "Pnts");
	for (int i = 0; i < pntsNode.childNodes().count(); ++i) {
		auto pNode = pntsNode.childNodes().at(i);
		QStringList frags = pNode.toElement().text().split(" ");
		double y = frags.at(0).toDouble();
		double x = frags.at(1).toDouble();
		double z = frags.at(2).toDouble();

		points->InsertNextPoint(x, y, 0);
		values->InsertNextValue(z);
	}
	auto facesNode = iRIC::getChildNode(defNode, "Faces");
	for (int i = 0; i < facesNode.childNodes().count(); ++i) {
		vtkIdType ids[3];
		auto fNode = facesNode.childNodes().at(i);
		QStringList frags = fNode.toElement().text().split(" ");
		ids[0] = frags.at(0).toInt() - 1;
		ids[1] = frags.at(1).toInt() - 1;
		ids[2] = frags.at(2).toInt() - 1;
		cellArray->InsertNextCell(3, ids);
	}
	polyData->SetPolys(cellArray);
	pmap->setSTLData(polyData, values);
	return true;
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
