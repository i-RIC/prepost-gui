#ifndef PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_H
#define PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_H

#include <crosssection/abstractcrosssectionwindow.h>

class PreProcessorGridDataItem;

class PreProcessorGridCrosssectionInternalWindow : public AbstractCrosssectionWindow
{
	Q_OBJECT

public:
	PreProcessorGridCrosssectionInternalWindow(PreProcessorGridDataItem* item, QWidget *parent);
	~PreProcessorGridCrosssectionInternalWindow();

	v4Structured2dGrid* grid() override;

private:
	ColorMapSettingContainerI* preColorMapSetting(const std::string& name) const override;

	PreProcessorGridDataItem* m_item;
};

#endif // PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_H
