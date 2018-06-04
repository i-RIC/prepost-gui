#ifndef MEASUREDDATAEXPORTERI_H
#define MEASUREDDATAEXPORTERI_H

class MeasuredData;

class QPointF;

class MeasuredDataExporterI
{
public:
	virtual ~MeasuredDataExporterI() {}

	virtual void exportData(const QString& filename, const QPointF& offset, const MeasuredData& data) const = 0;
};


#endif // MEASUREDDATAEXPORTERI_H

