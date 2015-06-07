#ifndef COLORMAPSETTINGWIDGET_H
#define COLORMAPSETTINGWIDGET_H

#include "guibase_global.h"

#include <QWidget>
#include <QColor>
#include <QList>

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
	/// Container for custom colormap setting
	class CustomSetting
	{

	public:
		/// Struct to store value-color pair in custom colormap setting.
		struct CustomColor {
			/// The value that corresponds to the color
			double value;
			/// The color
			QColor color;
		};
		/// Custom colormap type
		enum Type {
			tTwoColors,    ///< Color map with two colors
			tThreeColors,  ///< Color map with three colors
			tArbitrary     ///< Color map with arbitrary number colors
		};
		/// Color map type
		Type type;
		/// The color that corresponds to the maximum value
		QColor maxColor;
		/// The color that corresponds to the mid value
		QColor midColor;
		/// The color that corresponds to the minimum value
		QColor minColor;
		/// The mid value
		double midValue;

		QList<CustomColor> arbitrarySettings;

		CustomSetting() {
			type = tTwoColors;
			maxColor = Qt::red;
			minColor = Qt::blue;
			midColor = Qt::white;
			midValue = 0;
		}
	};
	ColorMapSettingWidget(QWidget* parent = nullptr);
	~ColorMapSettingWidget();
	/// The colormap type
	ColorMap colorMap();
	/// Set the colormap type
	void setColorMap(ColorMap colorMap);
	/// Disable the radio button to select custom colormap
	void disableCustom();
	/// Set the custom colormap setting
	void setCustomSetting(const CustomSetting& setting) {
		m_customSetting = setting;
	}
	/// The custom colormap setting
	const CustomSetting& customSetting() {
		return m_customSetting;
	}

private slots:
	void openCustomSettingDialog();

private:
	Ui::ColorMapSettingWidget* ui;

	CustomSetting m_customSetting;
};

#endif // COLORMAPSETTINGWIDGET_H
