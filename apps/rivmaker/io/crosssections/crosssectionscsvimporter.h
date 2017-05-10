#ifndef CROSSSECTIONSCSVIMPORTER_H
#define CROSSSECTIONSCSVIMPORTER_H

#include "crosssectionsimporteri.h"

#include <QObject>

class CrossSectionsCsvImporter : public QObject, public CrossSectionsImporterI
{
	Q_OBJECT

public:
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
	bool importData(std::vector<CrossSection*>* crossSections, QPointF* offset, const QString& filename, QWidget* w) override;
};

#endif // CROSSSECTIONSCSVIMPORTER_H
