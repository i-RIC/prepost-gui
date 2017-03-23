#ifndef CROSSSECTIONSEXPORTERI_H
#define CROSSSECTIONSEXPORTERI_H

#include <vector>

class CrossSection;

class QPointF;
class QString;
class QStringList;
class QWidget;

class CrossSectionsExporterI
{
public:
	virtual ~CrossSectionsExporterI() {}

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
	virtual bool exportData(const std::vector<CrossSection*>& crossSections, const QPointF& offset, const QString& filename, QWidget* w) = 0;
};

#endif // CROSSSECTIONSEXPORTERI_H
