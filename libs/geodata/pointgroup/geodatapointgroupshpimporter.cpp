#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupshpimporter.h"
#include "private/geodatapointgroup_impl.h"

#include <cs/coordinatesystemconverter.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
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

QPointF readPoint(SHPObject* shpo)
{
	return QPointF(*(shpo->padfX), *(shpo->padfY));
}

} // namespace

GeoDataPointGroupShpImporter::GeoDataPointGroupShpImporter(GeoDataCreator* creator) :
	GeoDataPolyDataGroupShpImporter {"esrishape_point_group", tr("ESRI Shapefile (Points)"), creator}
{}

const QStringList GeoDataPointGroupShpImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shapefile (Points) (*.shp)"));
	return ret;
}

bool GeoDataPointGroupShpImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPointGroup*>(data);

	std::string fname = iRIC::toStr(filename());
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	QString dbfFilename = filename();
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	int type;
	double minbound[4], maxbound[4];
	int count;

	SHPGetInfo(shph, &count, &type, minbound, maxbound);

	QString nameTpl = tr("Point%1");
	for (int i = 0; i < count; ++i) {
		SHPObject* shpo = SHPReadObject(shph, i);
		auto p = readPoint(shpo);

		if (m_converter != nullptr) {
			p = m_converter->convert(p);
		}

		auto point = new GeoDataPointGroupPoint(p, group);
		// name
		QString name = nameTpl.arg(i + 1);
		if (m_nameSetting == GeoDataPolyDataGroupShpImporterSettingDialog::nsLoadFromDBF) {
			name = readData(dbfh, i, m_nameAttribute, m_codec).toString();
		}
		point->setName(name);
		// value
		QVariant value = m_specifiedValue;
		if (m_valueSetting == GeoDataPolyDataGroupShpImporterSettingDialog::vsLoadFromDBF) {
			value = readData(dbfh, i, m_valueAttribute, m_codec);
		}
		point->setValue(value);
		group->addData(point);

		SHPDestroyObject(shpo);
	}
	SHPClose(shph);
	DBFClose(dbfh);

	group->setupDataItem();
	group->updateAttributeBrowser(true);
	group->updateVtkObjects();
	group->updateIndex();

	return true;
}
