#ifndef MEASUREDDATAPOINTSETTINGDIALOG_IMPL_H
#define MEASUREDDATAPOINTSETTINGDIALOG_IMPL_H

#include "../measureddatapointsettingdialog.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <vector>

class MeasuredDataPointSettingDialog::Impl
{
public:
	MeasuredDataPointSetting m_setting;

	std::unordered_map<std::string, QString> m_scalarBarTitleMap;
	std::unordered_map<std::string, LookupTableContainer*> m_lookupTables;
	std::vector<std::string> m_targets;

	LookupTableContainer m_lookupTable;
};

#endif // MEASUREDDATAPOINTSETTINGDIALOG_IMPL_H
