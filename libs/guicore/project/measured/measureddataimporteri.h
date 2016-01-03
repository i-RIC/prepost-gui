#ifndef MEASUREDDATAIMPORTERI_H
#define MEASUREDDATAIMPORTERI_H

class MeasuredData;

class MeasuredDataImporterI
{
public:
	virtual ~MeasuredDataImporterI() {}

	virtual MeasuredData* importData(const QString& filename) const = 0;
};

#endif // MEASUREDDATAIMPORTERI_H

