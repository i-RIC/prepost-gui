#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_IMPL_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_IMPL_H

#include "../preprocessorgridcrosssectionwindow2.h"
#include "preprocessorgridcrosssectionwindow2_controller.h"
#include "preprocessorgridcrosssectionwindow2_displaysettingcontainer.h"
#include "preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.h"

#include <unordered_map>
#include <vector>

class v4Structured2dGrid;
class RealNumberEditWidget;

class PreProcessorGridCrosssectionWindow2::Impl
{
public:
	Impl(PreProcessorGridDataItem* item, PreProcessorGridCrosssectionWindow2* w);
	~Impl();

	void setupSplitterSizes();
	void setupToolBars();
	void setupController();
	void setupDisplaySettingTableController();
	void setupEditTableController();
	void setupDisplaySettings();

	GraphicsView* graphicsView() const;
	v4Structured2dGrid* grid() const;
	void updateAspectRatioEdit();

	void setTargetDirection(Direction dir);
	ColorMapSettingContainerI* preColorMapSetting(const std::string& name);

	// display settings
	DisplaySettingContainer m_displaySetting;
	std::vector<GridAttributeDisplaySettingContainer> m_displaySettings;
	std::unordered_map<std::string, GridAttributeDisplaySettingContainer*> m_displaySettingMap;

	// View Toolbar
	QCheckBox* m_gridDisplayCheckBox;
	QCheckBox* m_scaleDisplayCheckBox;
	QCheckBox* m_markersDisplayCheckBox;
	QCheckBox* m_aspectRatioDisplayCheckBox;
	QPushButton* m_displaySettingButton;

	// Display Toolbar
	QCheckBox* m_autoRescaleCheckBox;
	QLabel* m_aspectRatioLabel;
	RealNumberEditWidget* m_aspectRatioEdit;
	QCheckBox* m_fixAspectRatioCheckBox;
	QCheckBox* m_fixRegionCheckBox;

	// Display Setting Controller
	DisplaySettingTableController* m_displaySettingTableController;
	EditTableController* m_editTableController;

	// Controller
	Controller* m_controller;

	PreProcessorGridDataItem* m_gridDataItem;
	PreProcessorGridCrosssectionWindow2* m_window;

	Direction m_tmpDirection;
	int m_tmpIndex;
	Controller::CellSide m_tmpCellSide;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_IMPL_H
