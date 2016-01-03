#ifndef MEASUREDDATAEXPORTERI_H
#define MEASUREDDATAEXPORTERI_H

class MeasuredData;

class MeasuredDataExporterI
{
public:
	virtual ~MeasuredDataExporterI() {}

	virtual void exportData(const QString& filename, const MeasuredData& data) const = 0;
};


#endif // MEASUREDDATAEXPORTERI_H

