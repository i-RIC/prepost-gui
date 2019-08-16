#ifndef LANDXMLUTIL_H
#define LANDXMLUTIL_H

#include "guibase_global.h"

class CoordinateSystem;

class QXmlStreamWriter;

class GUIBASEDLL_EXPORT LandXmlUtil
{
public:
	static void writeStartLandXml(QXmlStreamWriter* writer);
	static void writeProjectForMlit(QXmlStreamWriter* writer, const QString& name);
	static void writeApplication(QXmlStreamWriter* writer);
	static void writeUnits(QXmlStreamWriter* writer);
	static void writeCoordinateSystem(QXmlStreamWriter* writer, CoordinateSystem* cs);

private:
	LandXmlUtil();
};

#endif // LANDXMLUTIL_H
