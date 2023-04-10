#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H

#include "preprocessorrootdataitem.h"
#include <geodata/pointmap/geodatapointmapmappingmode.h>
#include "../misc/preprocessorgridattributemappingmode.h"

#include <QDialog>

namespace Ui
{
	class PreProcessorGridAttributeMappingSettingTopEditDialog;
}

class PreProcessorGridAttributeMappingSettingTopEditDialog : public QDialog
{
	Q_OBJECT

public:
	PreProcessorGridAttributeMappingSettingTopEditDialog(QWidget* parent = nullptr);
	~PreProcessorGridAttributeMappingSettingTopEditDialog();

	PreProcessorGridAttributeMappingMode::Mode mappingMode();
	void setMappingMode(PreProcessorGridAttributeMappingMode::Mode mm);

	Ui::PreProcessorGridAttributeMappingSettingTopEditDialog* ui;
};

#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H
