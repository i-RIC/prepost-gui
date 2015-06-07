#ifndef RAWDATABACKGROUNDCOMPLEX_H
#define RAWDATABACKGROUNDCOMPLEX_H

#include "rawdatabackground.h"

class PreProcessorRawDataComplexGroupDataItemInterface;

class RawDataBackgroundComplex : public RawDataBackground
{
	Q_OBJECT
public:
	/// Constructor
	RawDataBackgroundComplex(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridAttribute* att);
	void setItem(PreProcessorRawDataComplexGroupDataItemInterface* item) {
		m_item = item;
	}

	QVariant variantValue() override;

private:
	PreProcessorRawDataComplexGroupDataItemInterface* m_item;
};


#endif // RAWDATABACKGROUNDCOMPLEX_H
