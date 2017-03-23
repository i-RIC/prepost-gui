#ifndef CROSSSECTIONSEXPORTER_H
#define CROSSSECTIONSEXPORTER_H

#include <QObject>

#include <vector>

class CrossSection;

class CrossSectionsExporter : public QObject
{
	Q_OBJECT

public:
	static bool exportData(const std::vector<CrossSection*> crossSections, const QPointF& offset, QWidget* w);

private:
	CrossSectionsExporter();
};

#endif // CROSSSECTIONSEXPORTER_H
