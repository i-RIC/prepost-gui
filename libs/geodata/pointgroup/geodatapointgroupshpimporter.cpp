#include "geodatapointgroup.h"
#include "geodatapointgroupattributebrowser.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupshpimporter.h"
#include "private/geodatapointgroup_impl.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/informationdialog.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QMessageBox>
#include <QRectF>
#include <QTextCodec>
#include <QVariant>

#include <shapefil.h>

#include <geos/util/GEOSException.h>

namespace {

const double LOCATION_DELTA = 1.0E-6;

QVariant readData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec)
{
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

QPointF readPoint(SHPObject* shpo)
{
	return QPointF(*(shpo->padfX), *(shpo->padfY));
}

} // namespace

GeoDataPointGroupShpImporter::GeoDataPointGroupShpImporter(GeoDataCreator* creator) :
	GeoDataImporter {"esrishape_point_group", tr("ESRI Shapefile (Points)"), creator}
{}

GeoDataPointGroupShpImporter::~GeoDataPointGroupShpImporter()
{}

const QStringList GeoDataPointGroupShpImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shapefile (Points) (*.shp)"));
	return ret;
}

const QStringList GeoDataPointGroupShpImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("shp");
	return ret;
}

bool GeoDataPointGroupShpImporter::doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	auto fname = iRIC::toStr(filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_POINT) {
		QMessageBox::critical(w, tr("Error"), tr("The shape type contained in this shape file is not point."));
		return false;
	}
	auto dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	auto dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");
	if (dbfh == nullptr) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(dbfFilename)));
		return false;
	}
	int recordCount = DBFGetRecordCount(dbfh);

	if (numEntities != recordCount) {
		// wrong shape file.
		QMessageBox::critical(w, tr("Error"), tr("The number of poly lines mismatches between shp file and dbf file."));
		return false;
	}
	DBFClose(dbfh);

	m_count = numEntities;
	*count = 1;

	GridAttributeEditWidget* widget = condition->editWidget(0);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	GeoDataPointGroupShpImporterSettingDialog dialog(filename, widget, w);

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

bool GeoDataPointGroupShpImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	auto group = dynamic_cast<GeoDataPointGroup*>(data);

	std::string fname = iRIC::toStr(filename());
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	QString dbfFilename = filename();
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	QString nameTpl = tr("Point%1");
	for (int i = 0; i < m_count; ++i) {
		SHPObject* shpo = SHPReadObject(shph, i);
		auto p = readPoint(shpo);

		auto point = new GeoDataPointGroupPoint(p, group);
		// name
		QString name = nameTpl.arg(i + 1);
		if (m_nameSetting == GeoDataPointGroupShpImporterSettingDialog::nsLoadFromDBF) {
			name = readData(dbfh, i, m_nameAttribute, codec).toString();
		}
		point->setName(name);
		// value
		QVariant value = m_specifiedValue;
		if (m_valueSetting == GeoDataPointGroupShpImporterSettingDialog::vsLoadFromDBF) {
			value = readData(dbfh, i, m_valueAttribute, codec);
		}
		point->setValue(value);
		group->addPoint(point);

		SHPDestroyObject(shpo);
	}
	SHPClose(shph);
	DBFClose(dbfh);

	group->setupDataItem();
	group->impl->m_attributeBrowser->update();
	group->updateVtkObjects();
	group->updateIndex();

	return true;
}
