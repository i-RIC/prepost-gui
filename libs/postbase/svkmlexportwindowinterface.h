#ifndef SVKMLEXPORTWINDOWINTERFACE_H
#define SVKMLEXPORTWINDOWINTERFACE_H

#include <QList>

class QString;
class QXmlStreamWriter;

/// Window that support KML for street view.
class SVKmlExportWindowInterface
{
public:
	virtual ~SVKmlExportWindowInterface() {}

	virtual bool checkKmlExportCondition(const QString &zonename) const = 0;
	virtual bool exportKMLHeader(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLFooter(QXmlStreamWriter& writer, const QString& zonename) = 0;
	virtual bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time, const QString& zonename) = 0;
	virtual QList<QString> contourDrawingZones() = 0;
};

#endif // SVKMLEXPORTWINDOWINTERFACE_H
