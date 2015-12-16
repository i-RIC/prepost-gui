#ifndef COLORMAPSETTINGWIDGET_H
#define COLORMAPSETTINGWIDGET_H

#include "../guibase_global.h"

#include "colormapcustomsetting.h"

#include <QWidget>

namespace Ui
{
	class ColorMapSettingWidget;
}

/**
 * @brief Widget to edit color map setting
 *
 * User can select color map setting from the followings:
 * - Rainbow color
 * - Grayscale
 * - Custom
 * When custom ie selected, detailed setting can be edited
 * using ColorMapCustomSettingDialog.
 */
class GUIBASEDLL_EXPORT ColorMapSettingWidget : public QWidget
{
	Q_OBJECT

public:
	/// The types of colormap
	enum ColorMap {
		Rainbow,     ///< Rainbow color (from blue to red)
		Grayscale,   ///< Grayscale (from black to white)
		Custom       ///< Custom setting
	};

	ColorMapSettingWidget(QWidget* parent = nullptr);
	~ColorMapSettingWidget();

	ColorMap colorMap() const;
	void setColorMap(ColorMap colorMap);

	const ColorMapCustomSetting& customSetting() const;
	void setCustomSetting(const ColorMapCustomSetting& setting);

	void disableCustom();

private slots:
	void openCustomSettingDialog();

private:
	Ui::ColorMapSettingWidget* ui;

	ColorMapCustomSetting m_customSetting;
};

#endif // COLORMAPSETTINGWIDGET_H
