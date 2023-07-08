#ifndef ARROWSCOLORSETTINGEDITSMALLWIDGET_H
#define ARROWSCOLORSETTINGEDITSMALLWIDGET_H

#include "../guicore_global.h"
#include "../scalarstocolors/colormapsettingcontainer.h"

#include <QWidget>

#include <unordered_map>


namespace Ui {
class ArrowsColorSettingEditSmallWidget;
}

class ArrowsSettingContainer;
class SolverDefinitionGridType;

class GUICOREDLL_EXPORT ArrowsColorSettingEditSmallWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsColorSettingEditSmallWidget(QWidget *parent = nullptr);
	~ArrowsColorSettingEditSmallWidget();

	void setGridType(SolverDefinitionGridType* gridType);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings);

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

	QUndoCommand* colorMapModifyCommand() const;

private slots:
	void handleColorScalarChange(int index);
	void openColorMapEditDialog();

private:
	ColorMapSettingContainerI* m_colorMapSetting;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;

	std::vector<std::string> m_colorMapNames;
	SolverDefinitionGridType* m_gridType;
	Ui::ArrowsColorSettingEditSmallWidget *ui;
};

#endif // ARROWSCOLORSETTINGEDITSMALLWIDGET_H
