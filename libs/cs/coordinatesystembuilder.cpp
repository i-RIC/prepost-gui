#include "coordinatesystem.h"
#include "coordinatesystembuilder.h"

#include <QFile>
#include <QRegExp>
#include <QTextStream>

CoordinateSystemBuilder::CoordinateSystemBuilder(QObject* parent)
	: QObject(parent)
{
	QFile epsgFile(":/libs/cs/data/epsg.txt");
	epsgFile.open(QFile::ReadOnly);
	QTextStream s(&epsgFile);

	QRegExp rx("<([0-9]+)>(.+)<>");

	QString caption;
	while (! epsgFile.atEnd()) {
		QString name;
		QString def;
		QString line = s.readLine();
		if (line.at(0) == '#') {
			// Comment line. Save it as caption.
			caption = line.mid(1).trimmed();
		} else {
			// it is the definition.
			if (rx.exactMatch(line)) {
				name = QString("EPSG:%1").arg(rx.cap(1));
				def = rx.cap(2).trimmed();
			}
			CoordinateSystem* system = buildFromProj4String(def);
			if (! name.isEmpty() && ! caption.isEmpty() && system != nullptr) {
				system->setName(name);
				system->setCaption(caption);
				m_systems.append(system);
			} else {
				delete system;
			}
		}
	}
	/*
		// Japan Plane Rectangular CS I / EPSG:2443
		system = new CoordinateSystem("EPSG:2443", tr("Japan Plane Rectangular CS I"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=33 +lon_0=129.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS II / EPSG:2444
		system = new CoordinateSystem("EPSG:2444", tr("Japan Plane Rectangular CS II"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=33 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS III / EPSG:2445
		system = new CoordinateSystem("EPSG:2445", tr("Japan Plane Rectangular CS III"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=132.1666666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS IV / EPSG:2446
		system = new CoordinateSystem("EPSG:2446", tr("Japan Plane Rectangular CS IV"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=33 +lon_0=133.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS V / EPSG:2447
		system = new CoordinateSystem("EPSG:2447", tr("Japan Plane Rectangular CS V"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=134.3333333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS VI / EPSG:2448
		system = new CoordinateSystem("EPSG:2448", tr("Japan Plane Rectangular CS VI"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS VII / EPSG:2449
		system = new CoordinateSystem("EPSG:2449", tr("Japan Plane Rectangular CS VII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=137.1666666666667 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS VIII / EPSG:2450
		system = new CoordinateSystem("EPSG:2450", tr("Japan Plane Rectangular CS VIII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=138.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS IX / EPSG:2451
		system = new CoordinateSystem("EPSG:2451", tr("Japan Plane Rectangular CS IX"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=36 +lon_0=139.8333333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS X / EPSG:2452
		system = new CoordinateSystem("EPSG:2452", tr("Japan Plane Rectangular CS X"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=40 +lon_0=140.8333333333333 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XI / EPSG:2453
		system = new CoordinateSystem("EPSG:2453", tr("Japan Plane Rectangular CS XI"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=44 +lon_0=140.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XII / EPSG:2454
		system = new CoordinateSystem("EPSG:2454", tr("Japan Plane Rectangular CS XII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=44 +lon_0=142.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XIII / EPSG:2455
		system = new CoordinateSystem("EPSG:2455", tr("Japan Plane Rectangular CS XIII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=44 +lon_0=144.25 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XIV / EPSG:2456
		system = new CoordinateSystem("EPSG:2456", tr("Japan Plane Rectangular CS XIV"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=26 +lon_0=142 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XV / EPSG:2457
		system = new CoordinateSystem("EPSG:2457", tr("Japan Plane Rectangular CS XV"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=26 +lon_0=127.5 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XVI / EPSG:2458
		system = new CoordinateSystem("EPSG:2458", tr("Japan Plane Rectangular CS XVI"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=26 +lon_0=124 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XVII / EPSG:2459
		system = new CoordinateSystem("EPSG:2459", tr("Japan Plane Rectangular CS XVII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=26 +lon_0=131 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XVIII / EPSG:2460
		system = new CoordinateSystem("EPSG:2460", tr("Japan Plane Rectangular CS XVIII"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=20 +lon_0=136 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
		// Japan Plane Rectangular CS XIX / EPSG:2461
		system = new CoordinateSystem("EPSG:2461", tr("Japan Plane Rectangular CS XIX"), "+proj=latlong +ellps=GRS80", "+proj=tmerc +lat_0=26 +lon_0=154 +k=0.9999 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs");
		m_systems.append(system);
	*/
	setupMap();
}


CoordinateSystem* CoordinateSystemBuilder::buildFromProj4String(const QString& line)
{
	QRegExp rx("<([0-9]+)>(.+)<>");
	QRegExp rx2("\\+ellps=([a-zA-Z0-9_\\-]+) ");
	QRegExp rx3("\\+datum=([a-zA-Z0-9_\\-]+) ");

	QString def;
	QString lonlatdef;
	bool ellpsFound = false;
	bool datumFound = false;
	def = line;
	lonlatdef = "+proj=latlong";
	if (rx2.indexIn(line) != -1) {
		QString ellps = rx2.cap(1);
		lonlatdef.append(QString(" +ellps=%1").arg(ellps));
		ellpsFound = true;
	}
	if (rx3.indexIn(line) != -1) {
		QString datum = rx3.cap(1);
		lonlatdef.append(QString(" +datum=%1").arg(datum));
		datumFound = true;
	}
	if (! ellpsFound && ! datumFound) {
		return 0;
	}
	CoordinateSystem* ret = new CoordinateSystem("", "", lonlatdef.trimmed(), def.trimmed());
	ret->init();
	return ret;
}

CoordinateSystemBuilder::~CoordinateSystemBuilder()
{
	for (auto s : m_systems) {
		delete s;
	}
}

const QList<CoordinateSystem*>& CoordinateSystemBuilder::systems() const
{
	return m_systems;
}

CoordinateSystem* CoordinateSystemBuilder::system(const QString& name) const
{
	return m_systemsMap.value(name, 0);
}

void CoordinateSystemBuilder::setupMap()
{
	for (auto s : m_systems) {
		m_systemsMap.insert(s->name(), s);
	}
}
