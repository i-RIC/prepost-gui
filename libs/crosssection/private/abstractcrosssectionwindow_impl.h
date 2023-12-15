#ifndef ABSTRACTCROSSSECTIONWINDOW_IMPL_H
#define ABSTRACTCROSSSECTIONWINDOW_IMPL_H

#include "../abstractcrosssectionwindow.h"
#include "../public/abstractcrosssectionwindow_controller.h"
#include "abstractcrosssectionwindow_displaysettingcontainer.h"
#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"

#include <unordered_map>
#include <vector>

class RealNumberEditWidget;
class v4Structured2dGrid;

class QCheckBox;
class QLabel;
class QToolBar;
class QPushButton;

class AbstractCrosssectionWindow::Impl
{
public:
	Impl(AbstractCrosssectionWindow* w, QWidget* parent);
	~Impl();

	void setupSplitterSizes();
	void setupToolBars();
	void setupController(QWidget* parent);
	void setupDisplaySettingTableController();
	void setupEditTableController();
	void setupDisplaySettings();

	GraphicsView* graphicsView() const;
	void updateColorMapValueRanges();
	void updateAspectRatioEdit();

	void setTargetDirection(Direction dir);

	// display settings
	DisplaySettingContainer m_displaySetting;
	std::vector<GridAttributeDisplaySettingContainer> m_displaySettings;
	std::unordered_map<std::string, GridAttributeDisplaySettingContainer*> m_displaySettingMap;

	// View Toolbar
	QToolBar* m_viewToolBar;
	QCheckBox* m_gridDisplayCheckBox;
	QCheckBox* m_scaleDisplayCheckBox;
	QCheckBox* m_markersDisplayCheckBox;
	QCheckBox* m_aspectRatioDisplayCheckBox;
	QPushButton* m_displaySettingButton;

	// Display Toolbar
	QToolBar* m_displayToolBar;
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

	AbstractCrosssectionWindow* m_window;

	Direction m_tmpDirection;
	int m_tmpIndex;
	Controller::CellSide m_tmpCellSide;

private:
	void setupDisplaySettings(v4Structured2dGrid* grid, const QString& prefix);
};

#endif // ABSTRACTCROSSSECTIONWINDOW_IMPL_H
