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

class GUICOREDLL_EXPORT ArrowsColorSettingEditSmallWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsColorSettingEditSmallWidget(QWidget *parent = nullptr);
	~ArrowsColorSettingEditSmallWidget();

	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings);
	void setColorMapSettings(std::unordered_map<std::string, ColorMapSettingContainer>* settings);

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

	QUndoCommand* colorMapModifyCommand() const;

private slots:
	void handleColorScalarChange(int index);
	void openColorMapEditDialog();

private:
	enum class ColorMapMode {
		None,
		Copy,
		Pointer
	};

	ColorMapSettingContainer m_colorMapSetting;
	std::unordered_map<std::string, ColorMapSettingContainer>* m_copyColorMapSettings;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_pointerColorMapSettings;
	ColorMapMode m_mode;

	std::vector<std::string> m_colorMapNames;

	Ui::ArrowsColorSettingEditSmallWidget *ui;
};

#endif // ARROWSCOLORSETTINGEDITSMALLWIDGET_H
