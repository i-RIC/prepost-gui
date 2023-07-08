#ifndef ARROWSCOLORSETTINGEDITWIDGET_H
#define ARROWSCOLORSETTINGEDITWIDGET_H

#include <QWidget>

#include <unordered_map>

namespace Ui {
class ArrowsColorSettingEditWidget;
}

class ArrowsSettingContainer;
class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class SolverDefinitionGridType;

class ArrowsColorSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsColorSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsColorSettingEditWidget();

	void setGridType(SolverDefinitionGridType* gridType);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings);

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

	QUndoCommand* colorMapModifyCommand() const;

private slots:
	void handleColorScalarChange(int index);

private:
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	std::vector<std::string> m_colorMapNames;

	ColorMapSettingEditWidgetI* m_colorMapWidget;
	SolverDefinitionGridType* m_gridType;
	Ui::ArrowsColorSettingEditWidget *ui;
};

#endif // ARROWSCOLORSETTINGEDITWIDGET_H
