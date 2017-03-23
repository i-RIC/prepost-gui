#ifndef CROSSSECTIONSCSVEXPORTER_H
#define CROSSSECTIONSCSVEXPORTER_H

#include "crosssectionsexporteri.h"

#include <QObject>

class CrossSectionsCsvExporter : public QObject, public CrossSectionsExporterI
{
	Q_OBJECT

public:
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
	bool exportData(const std::vector<CrossSection*>& crossSections, const QPointF& offset, const QString& filename, QWidget* w) override;
};

#endif // CROSSSECTIONSCSVEXPORTER_H
