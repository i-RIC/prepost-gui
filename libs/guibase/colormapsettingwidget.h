#ifndef COLORMAPSETTINGWIDGET_H
#define COLORMAPSETTINGWIDGET_H

#include "guibase_global.h"

#include <QWidget>
#include <QColor>
#include <QList>

namespace Ui {
	class ColorMapSettingWidget;
}

class GUIBASEDLL_EXPORT ColorMapSettingWidget : public QWidget {
	Q_OBJECT

public:
	enum ColorMap {Rainbow, Grayscale, Custom};
	class CustomSetting {
	public:
		struct CustomColor {
			double value;
			QColor color;
		};

		enum Type {
			tTwoColors,
			tThreeColors,
			tArbitrary
		};

		Type type;

		QColor maxColor;
		QColor midColor;
		QColor minColor;
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
	ColorMapSettingWidget(QWidget *parent = 0);
	~ColorMapSettingWidget();
	void setColorMap(ColorMap colorMap);
	ColorMap colorMap();
	void disableCustom();
	void setCustomSetting(const CustomSetting& setting) {
		m_customSetting = setting;
	}
	const CustomSetting& customSetting() {
		return m_customSetting;
	}

private slots:
	void openCustomSettingDialog();

private:
	Ui::ColorMapSettingWidget *ui;

	CustomSetting m_customSetting;
};

#endif // COLORMAPSETTINGWIDGET_H
