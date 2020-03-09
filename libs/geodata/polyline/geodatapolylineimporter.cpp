#include "geodatapolyline.h"
#include "geodatapolylineimplpolyline.h"
#include "geodatapolylineimporter.h"
#include "geodatapolylineimportersettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <misc/errormessage.h>
#include <misc/informationdialog.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QMessageBox>
#include <QStringList>
#include <QTextCodec>

#include <shapefil.h>

#include <geos/geom/LineString.h>
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

std::vector<QPointF> readPolyLine(SHPObject* shpo)
{
	std::vector<QPointF> ret;

	int start = *(shpo->panPartStart);
	int end = shpo->nVertices;
	if (shpo->nParts != 1) {
		end = *(shpo->panPartStart + 1);
	}
	QPointF lastPoint;
	for (int i = start; i < end; ++i) {
		QPointF point(*(shpo->padfX + i), *(shpo->padfY + i));
		QPointF diff = lastPoint - point;
		bool veryNear = (std::fabs(diff.x()) < LOCATION_DELTA && std::fabs(diff.y()) < LOCATION_DELTA);
		if (i != start && veryNear) {continue;}
		ret.push_back(point);
		lastPoint = point;
	}
	return ret;
}

} // namespace

GeoDataPolyLineImporter::GeoDataPolyLineImporter(GeoDataCreator* creator) :
	GeoDataImporter {"esrishape", tr("ESRI Shapefile (Polyline)"), creator}
{}

GeoDataPolyLineImporter::~GeoDataPolyLineImporter()
{}

const QStringList GeoDataPolyLineImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shapefile (*.shp) (Polyline)"));
	return ret;
}

const QStringList GeoDataPolyLineImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("shp");
	return ret;
}

bool GeoDataPolyLineImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_ARC) {
		QMessageBox::critical(w, tr("Error"), tr("The shape type contained in this shape file is not polyline."));
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
		QMessageBox::critical(w, tr("Error"), tr("The number of polylines mismatches between shp file and dbf file."));
		return false;
	}
	DBFClose(dbfh);

	m_count = numEntities;
	*count = numEntities;

	GridAttributeEditWidget* widget = condition->editWidget(nullptr);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	GeoDataPolyLineImporterSettingDialog dialog(filename, widget, w);

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

bool GeoDataPolyLineImporter::importData(GeoData* data, int index, QWidget* /*w*/)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	GeoDataPolyLine* poly = dynamic_cast<GeoDataPolyLine*>(data);

	std::string fname = iRIC::toStr(filename());
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	SHPObject* shpo = SHPReadObject(shph, index);

	auto polyline = readPolyLine(shpo);

	poly->polyLine()->setPolyLine(polyline);

	SHPDestroyObject(shpo);
	SHPClose(shph);

	QString dbfFilename = filename();
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	// read name.
	if (m_nameSetting == GeoDataPolyLineImporterSettingDialog::nsLoadFromDBF) {
		QVariant name = readData(dbfh, index, m_nameAttribute, codec);
		poly->setCaption(name.toString());
	}
	// read value.
	QVariant value;
	if (m_valueSetting == GeoDataPolyLineImporterSettingDialog::vsLoadFromDBF) {
		value = readData(dbfh, index, m_valueAttribute, codec);
	} else {
		value = m_specifiedValue;
	}
	DBFClose(dbfh);

	poly->setVariantValue(value, index != (m_count - 1));
	poly->restoreMouseEventMode();
	poly->setupDataItem();

	return true;
}
