#include "rawdatapolygon.h"
#include "rawdatapolygonholepolygon.h"
#include "rawdatapolygonimporter.h"
#include "rawdatapolygonimportersettingdialog.h"

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioneditwidget.h>
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QMessageBox>
#include <QStringList>
#include <QTextCodec>

#include <shapefil.h>

const QStringList RawDataPolygonImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shape file (*.shp)"));
	return ret;
}

const QStringList RawDataPolygonImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("shp");
	return ret;
}

bool RawDataPolygonImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w)
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
	*count = numEntities;
	m_count = numEntities;

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");
	int recordCount = DBFGetRecordCount(dbfh);

	if (numEntities != recordCount) {
		// wrong shape file.
		QMessageBox::critical(w, tr("Error"), tr("The number of polygons mismatches between shp file and dbf file."));
		return false;
	}
	DBFClose(dbfh);
	GridRelatedConditionEditWidget* widget = condition->editWidget(0);
	item->setupEditWidget(widget);
	widget->setVariantValue(condition->variantDefaultValue());
	RawDataPolygonImporterSettingDialog dialog(filename, widget, w);

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

bool RawDataPolygonImporter::importData(RawData* data, int index, QWidget* w)
{
	QTextCodec* codec = QTextCodec::codecForLocale();
	RawDataPolygon* poly = dynamic_cast<RawDataPolygon*>(data);

	std::string fname = iRIC::toStr(m_filename);
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	SHPObject* shpo = SHPReadObject(shph, index);

	// Currently, polygon without holes are supported.
	QList<QPolygonF> holes;
	QPolygonF region;
	RawDataPolygonHolePolygon* tmpp = new RawDataPolygonHolePolygon(poly);
	for (int i = 0; i < shpo->nParts; ++i) {
		int start = *(shpo->panPartStart + i);
		int end = shpo->nVertices;
		if (i != shpo->nParts - 1) {
			end = *(shpo->panPartStart + i + 1);
		}
		QPolygonF tmppoly;
		for (int j = start; j < end; ++j) {
			tmppoly.append(QPointF(*(shpo->padfX + j), *(shpo->padfY + j)));
		}
		if (tmppoly.at(0) != tmppoly.at(tmppoly.count() - 1)) {
			tmppoly.append(tmppoly.at(0));
		}
		tmpp->setPolygon(tmppoly);
		if (shpo->nParts == 1 || tmpp->isClockwise()) {
			region = tmppoly;
		} else {
			holes.append(tmppoly);
		}
	}
	delete tmpp;
	try {
		poly->setPolygon(region);
		for (int i = 0; i < holes.count(); ++i) {
			poly->addHolePolygon(holes.at(i));
		}
	} catch (ErrorMessage& msg) {
		QMessageBox::warning(w, tr("Warning"), tr("Importing Polygon failed: %1").arg(msg));
		return false;
	}

	SHPDestroyObject(shpo);
	SHPClose(shph);

	QString dbfFilename = m_filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	// read name.
	if (m_nameSetting == RawDataPolygonImporterSettingDialog::nsLoadFromDBF) {
		QVariant name = readData(dbfh, index, m_nameAttribute, codec);
		poly->setCaption(name.toString());
	}
	// read value.
	QVariant value;
	if (m_valueSetting == RawDataPolygonImporterSettingDialog::vsLoadFromDBF) {
		value = readData(dbfh, index, m_valueAttribute, codec);
	} else {
		value = m_specifiedValue;
	}
	poly->setVariantValue(value);
	poly->restoreMouseEventMode();
	poly->setupDataItem();
	poly->updateGrid(index != (m_count - 1));

	DBFClose(dbfh);
	return true;
}
