#ifndef COLORMAPSETTINGWIDGET_H
#define COLORMAPSETTINGWIDGET_H

#include "guibase_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>

#include <QWidget>
#include <QList>

class QDomNode;
class QXmlStreamWriter;

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
	class GUIBASEDLL_EXPORT CustomSetting
	{

	public:
		/// Struct to store value-color pair in custom colormap setting.
		struct CustomColor : public CompositeContainer
		{
			/// Constructor
			CustomColor();
			/// Copy constructor
			CustomColor(const CustomColor& c);
			/// Copy operator
			CustomColor& operator=(const CustomColor& c);

			/// The value that corresponds to the color
			DoubleContainer value;
			/// The color
			ColorContainer color;
		};
		/// Custom colormap type
		enum Type {
			tTwoColors,    ///< Color map with two colors
			tThreeColors,  ///< Color map with three colors
			tArbitrary     ///< Color map with arbitrary number colors
		};
		/// Color map type
		EnumContainerT<Type> type;
		/// The color that corresponds to the maximum value
		ColorContainer maxColor;
		/// The color that corresponds to the mid value
		ColorContainer midColor;
		/// The color that corresponds to the minimum value
		ColorContainer minColor;
		/// The mid value
		DoubleContainer midValue;

		QList<CustomColor> arbitrarySettings;

		CustomSetting();
		void load(const QDomNode&);
		void save(QXmlStreamWriter&) const;
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
