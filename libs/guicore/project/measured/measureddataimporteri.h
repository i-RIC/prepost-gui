#ifndef MEASUREDDATAIMPORTERI_H
#define MEASUREDDATAIMPORTERI_H

class MeasuredData;
class ProjectDataItem;

class MeasuredDataImporterI
{
public:
	virtual ~MeasuredDataImporterI() {}

	virtual MeasuredData* importData(const QString& filename, ProjectDataItem* parent) const = 0;
};

#endif // MEASUREDDATAIMPORTERI_H
