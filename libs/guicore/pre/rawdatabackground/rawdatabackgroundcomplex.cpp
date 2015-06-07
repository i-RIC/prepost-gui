#include "../base/preprocessorrawdatacomplexgroupdataiteminterface.h"
#include "../complex/gridcomplexconditionwidget.h"
#include "rawdatabackgroundcomplex.h"

RawDataBackgroundComplex::RawDataBackgroundComplex(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridAttribute* att)
	: RawDataBackground(d, creator, att)
{

}

QVariant RawDataBackgroundComplex::variantValue()
{
	int defaultVal = 1;
	for (int i = 0; i < m_item->m_widgets.count(); ++i) {
		if (m_item->m_widgets.at(i)->isDefault()) {
			defaultVal = i + 1;
		}
	}
	return defaultVal;
}
