#include "geodatapolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygonimporter.h"
#include "geodatapolygonimportersettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <misc/errormessage.h>
#include <misc/informationdialog.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QMessageBox>
#include <QStringList>
#include <QTextCodec>

#include <shapefil.h>

#include <geos/geom/Polygon.h>
#include <geos/util/GEOSException.h>

#include <algorithm>
#include <cmath>

namespace {

const double LOCATION_DELTA = 1.0E-6;

QVariant readData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec) {
	DBFFieldType type = DBFGetFieldInfo(handle, fieldid, NULL, NULL, NULL);
	QVariant val;
	if (type == FTString) {
		QString strval = codec->toUnicode(DBFReadStringAttribute(handle, dataid, fieldid));
		val = strval;
	} else if (type == FTInteger) {
		int intval = DBFReadIntegerAttribute(handle, dataid, fieldid);
		val = intval;
	} else if (type == FTDouble) {
		double doubleval = DBFReadDoubleAttribute(handle, dataid, fieldid);
		val = doubleval;
	} else if (type == FTLogical) {
		QString logval = DBFReadLogicalAttribute(handle, dataid, fieldid);
		val = logval;
	}
	return val;
}

bool isRectContained(const QRectF& rbig, const QRectF& rsmall)
{
	return (rbig.left() <= rsmall.left() && rbig.right() >= rsmall.right() && rbig.top() <= rsmall.top() && rbig.bottom() >= rsmall.bottom());
}

bool isPolygonContained(const QPolygonF& pbig, const QPolygonF& psmall)
{
	for (int i = 0; i < psmall.size(); ++i) {
		QPointF p = psmall.at(i);
		if (! pbig.containsPoint(p, Qt::OddEvenFill)) {
			return false;
		}
	}
	return true;
}

struct RectIndex
{
	RectIndex(const QRectF& r, const QPolygonF& p, int idx) :
		rect(r), polygon(p), index(idx)
	{}
	QRectF rect;
	QPolygonF polygon;
	int index;
};

struct RectIndexAreaReverseSorter
{
	bool operator() (const RectIndex& r1, const RectIndex& r2)
	{
		double r1Area = r1.rect.width() * r1.rect.height();
		double r2Area = r2.rect.width() * r2.rect.height();
		return r1Area > r2Area;
	}
};

QPolygonF readPolygon(SHPObject* shpo, int partIndex)
{
	QPolygonF ret;

	int start = *(shpo->panPartStart + partIndex);
	int end = shpo->nVertices;
	if (partIndex != shpo->nParts - 1) {
		end = *(shpo->panPartStart + partIndex + 1);
	}
	QPointF lastPoint;
	for (int i = start; i < end; ++i) {
		QPointF point(*(shpo->padfX + i), *(shpo->padfY + i));
		QPointF diff = lastPoint - point;
		bool veryNear = (std::fabs(diff.x()) < LOCATION_DELTA && std::fabs(diff.y()) < LOCATION_DELTA);
		if (i != start && veryNear) {continue;}
		ret.append(point);
		lastPoint = point;
	}
	if (ret.at(0) != ret.at(ret.count() - 1)){
		ret.append(ret.at(0));
	}
	return ret;
}

} // namespace

GeoDataPolygonImporter::GeoDataPolygonImporter(GeoDataCreator* creator) :
	GeoDataImporter {"esrishape", tr("ESRI Shape"), creator}
{}

GeoDataPolygonImporter::~GeoDataPolygonImporter()
{}

const QStringList GeoDataPolygonImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shape file (*.shp)"));
	return ret;
}

const QStringList GeoDataPolygonImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("shp");
	return ret;
}

bool GeoDataPolygonImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_POLYGON) {
		QMessageBox::critical(w, tr("Error"), tr("The shape type contained in this shape file is not polygon."));
		return false;
	}
	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");
	if (dbfh == nullptr) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(dbfFilename)));
		return false;
	}
	int recordCount = DBFGetRecordCount(dbfh);

	if (numEntities != recordCount) {
		// wrong shape file.
		QMessageBox::critical(w, tr("Error"), tr("The number of polygons mismatches between shp file and dbf file."));
		return false;
	}
	DBFClose(dbfh);

	m_shapeInfos = buildPolygonShapeInfos(fname);
	*count = static_cast<int>(m_shapeInfos.size());

	GridAttributeEditWidget* widget = condition->editWidget(0);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	GeoDataPolygonImporterSettingDialog dialog(filename, widget, w);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	m_nameSetting = dialog.nameSetting();
	m_nameAttribute = dialog.nameIndex();
	m_valueSetting = dialog.valueSetting();
	m_valueAttribute = dialog.valueIndex();
	m_specifiedValue = dialog.specifiedValue();
	return true;
}

bool GeoDataPolygonImporter::importData(GeoData* data, int index, QWidget* w)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	GeoDataPolygon* poly = dynamic_cast<GeoDataPolygon*>(data);

	PolygonShapeInfo info = m_shapeInfos.at(index);

	std::string fname = iRIC::toStr(filename());
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	SHPObject* shpo = SHPReadObject(shph, info.item);

	QPolygonF region = readPolygon(shpo, info.region);
	std::vector<QPolygonF> holes;
	for (int i = 0; i < info.holes.size(); ++i) {
		int holeIndex = info.holes.at(i);
		holes.push_back(readPolygon(shpo, holeIndex));
	}

	QString warningMessage = tr(
				"%1 th polygon can not be imported. Polygon like below can not be imported:\n"
				"- It has less than three points\n"
				"- It is not closed\n"
				"- Lines of polygon intersect each other\n"
				"- Hole polygon is outside of region polygon\n"
				"- Polygon passes the same point several times"
				).arg(index + 1);

	try {
		poly->regionPolygon()->setPolygon(region);
		for (int i = 0; i < holes.size(); ++i){
			poly->addHolePolygon(holes.at(i));
		}
		geos::geom::Polygon* polygon = poly->getGeosPolygon(QPointF(0, 0));
		delete polygon;
	} catch (geos::util::GEOSException&){
		InformationDialog::warning(w, tr("Warning"), warningMessage, "polygon_import_warn");
		return false;
	} catch (ErrorMessage&) {
		InformationDialog::warning(w, tr("Warning"), warningMessage, "polygon_import_warn");
		return false;
	}

	SHPDestroyObject(shpo);
	SHPClose(shph);

	QString dbfFilename = filename();
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	// read name.
	if (m_nameSetting == GeoDataPolygonImporterSettingDialog::nsLoadFromDBF) {
		QVariant name = readData(dbfh, info.item, m_nameAttribute, codec);
		poly->setCaption(name.toString());
	}
	// read value.
	QVariant value;
	if (m_valueSetting == GeoDataPolygonImporterSettingDialog::vsLoadFromDBF) {
		value = readData(dbfh, info.item, m_valueAttribute, codec);
	} else {
		value = m_specifiedValue;
	}
	DBFClose(dbfh);

	poly->setVariantValue(value, index != (m_shapeInfos.size() - 1));
	poly->restoreMouseEventMode();
	poly->setupDataItem();
	poly->updatePolyData(index != (m_shapeInfos.size() - 1));

	return true;
}

std::vector<GeoDataPolygonImporter::PolygonShapeInfo> GeoDataPolygonImporter::buildPolygonShapeInfos(const std::string& shpFileName)
{
	SHPHandle shph = SHPOpen(shpFileName.c_str(), "rb");
	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	std::vector<PolygonShapeInfo> ret;

	for (int i = 0; i < numEntities; ++i) {
		std::vector<RectIndex> rectVec;
		std::vector<bool> holeVec;

		SHPObject* shpo = SHPReadObject(shph, i);
		if (shpo == 0) {
			// read error. skip this data.
			continue;
		}

		for (int j = 0; j < shpo->nParts; ++j){
			QPolygonF tmppoly = readPolygon(shpo, j);
			QRectF rect = tmppoly.boundingRect();
			rectVec.push_back(RectIndex(rect, tmppoly, j));
			holeVec.push_back(false);
		}
		std::sort(rectVec.begin(), rectVec.end(), RectIndexAreaReverseSorter());

		for (int j = 0; j < rectVec.size(); ++j) {
			bool hole = holeVec.at(j);
			if (hole) {continue;}
			const RectIndex& ri = rectVec.at(j);
			PolygonShapeInfo info;
			QRectF regionRect = ri.rect;
			QPolygonF regionPolygon = ri.polygon;
			info.item = i;
			info.region = ri.index;

			for (int k = j + 1; k < rectVec.size(); ++k) {
				const RectIndex& ri2 = rectVec.at(k);
				QRectF holeRect = ri2.rect;
				QPolygonF holePolygon = ri2.polygon;
				if (isRectContained(regionRect, holeRect) && isPolygonContained(regionPolygon, holePolygon)){
					info.holes.push_back(ri2.index);
					holeVec[k] = true;
				}
			}
			ret.push_back(info);
		}
		SHPDestroyObject(shpo);
	}
	SHPClose(shph);
	return ret;
}
