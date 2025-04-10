#include "geodatapolydatagroupcreator.h"
#include "geodatapolydatagroupshpimporter.h"
#include "private/geodatapolydatagroupshpimporter_importersetting.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemconverter.h>
#include <cs/gdalutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/geodata/geodataimportersetting.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QMessageBox>
#include <QTextCodec>

#include <shapefil.h>

GeoDataPolyDataGroupShpImporter::GeoDataPolyDataGroupShpImporter(const std::string& name, const QString& caption, GeoDataCreator* creator) :
	GeoDataImporter {name, caption, creator},
	m_codec {nullptr},
	m_converter {nullptr}
{}

GeoDataPolyDataGroupShpImporter::~GeoDataPolyDataGroupShpImporter()
{
	delete m_converter;
}

const QStringList GeoDataPolyDataGroupShpImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("shp");
	return ret;
}

GeoDataImporterSetting* GeoDataPolyDataGroupShpImporter::createSetting() const
{
	return new ImporterSetting();
}

QVariant GeoDataPolyDataGroupShpImporter::readData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec)
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

bool GeoDataPolyDataGroupShpImporter::doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto c = dynamic_cast<GeoDataPolyDataGroupCreator*> (creator());

	auto filename = setting()->fileName();
	auto fname = iRIC::toStr(filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(shph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != c->shapeType()) {
		QMessageBox::critical(w, tr("Error"), tr("The shape type contained in this shape file is not %1.").arg(c->shapeName()));
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
		QMessageBox::critical(w, tr("Error"), tr("The number of data mismatches between shp file and dbf file."));
		return false;
	}
	DBFClose(dbfh);

	// m_shapeInfos = buildPolygonShapeInfos(fname);
	*count = 1;

	GridAttributeEditWidget* widget = condition->editWidget(0);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	GeoDataPolyDataGroupShpImporterSettingDialog dialog(filename, widget, w);
	dialog.setWindowTitle(tr("%1 Group Import Setting").arg(c->shapeNameCamelCase()));
	if (condition->isReferenceInformation()) {
		dialog.hideValueWidgets();
	}

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();
	dialog.setCsBuilder(csBuilder);
	auto prjFilename = filename;
	prjFilename.replace(QRegExp(".shp$"), ".prj");
	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	if (QFile::exists(prjFilename)) {
		// read and get EPSG code
		QFile f(prjFilename);
		f.open(QFile::ReadOnly);
		auto wkt = f.readAll().toStdString();
		int epsgCode = GdalUtil::wkt2Epsg(wkt.c_str());
		auto cs = csBuilder->system(QString("EPSG:%1").arg(epsgCode));
		if (cs != nullptr) {
			dialog.setCoordinateSystem(cs);
		} else {
			dialog.setCoordinateSystem(projectCs);
		}
	} else {
		dialog.setCoordinateSystem(projectCs);
	}
	dialog.setCsEnabled(projectCs != nullptr);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}

	m_nameSetting = dialog.nameSetting();
	m_nameAttribute = dialog.nameIndex();
	m_valueSetting = dialog.valueSetting();
	m_valueAttribute = dialog.valueIndex();
	m_specifiedValue = dialog.specifiedValue();
	m_codec = QTextCodec::codecForName(dialog.codecName().toLatin1());
	auto cs = dialog.coordinateSystem();

	QStringList labels = dialog.getLabels(m_codec);
	m_nameAttributeName = labels.at(m_nameAttribute);
	m_valueAttributeName = labels.at(m_valueAttribute);

	if (projectCs != nullptr && projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}

	auto s = dynamic_cast<ImporterSetting*>(setting());
	s->nameSetting = m_nameSetting;
	if (m_nameSetting == GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting::nsLoadFromDBF) {
		s->nameAttribute = m_nameAttributeName;
	} else {
		s->nameAttribute = "";
	}
	s->valueSetting = m_valueSetting;
	if (m_valueSetting == GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting::vsLoadFromDBF) {
		s->valueAttribute = m_valueAttributeName;
		s->specifiedValue = "";
	} else {
		s->valueAttribute = "";
		s->specifiedValue = m_specifiedValue.toString();
	}
	s->codecName = m_codec->name();
	s->csName = cs->name();

	return true;
}

bool GeoDataPolyDataGroupShpImporter::doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto c = dynamic_cast<GeoDataPolyDataGroupCreator*> (creator());

	auto filename = setting()->fileName();
	auto s = dynamic_cast<ImporterSetting*> (setting());

	*count = 1;

	m_nameSetting = s->nameSetting;
	m_nameAttributeName = s->nameAttribute;
	m_valueSetting = s->valueSetting;
	m_valueAttributeName = s->valueAttribute;
	m_specifiedValue = s->specifiedValue.value();
	m_codec = QTextCodec::codecForName(s->codecName.value().toLatin1());

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();

	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	auto cs = csBuilder->system(s->csName);
	if (projectCs != nullptr && projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}

	GridAttributeEditWidget* widget = condition->editWidget(0);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	GeoDataPolyDataGroupShpImporterSettingDialog dialog(filename, widget, w);

	auto labels = dialog.getLabels(m_codec);
	m_nameAttribute = labels.indexOf(m_nameAttributeName);
	m_valueAttribute = labels.indexOf(m_valueAttributeName);

	return true;
}

