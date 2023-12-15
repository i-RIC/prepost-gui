#ifndef ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGCONTAINER_H
#define ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGCONTAINER_H

#include "../abstractcrosssectionwindow.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/qfontcontainer.h>
#include <misc/boolcontainer.h>

class AbstractCrosssectionWindow::DisplaySettingContainer : public CompositeContainer
{
public:
	enum class BackgroundGridType {
		Lines,
		Dots
	};

	DisplaySettingContainer();
	DisplaySettingContainer(const DisplaySettingContainer& setting);

	DisplaySettingContainer& operator=(const DisplaySettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void applyFromWidgets(AbstractCrosssectionWindow::Impl* impl);
	void applyToWidgets(AbstractCrosssectionWindow::Impl* impl);

	BoolContainer gridVisible;
	BoolContainer scaleVisible;
	BoolContainer markersVisible;
	BoolContainer aspectRatioVisible;

	BoolContainer enableAutoRescale;
	BoolContainer fixAspectRatio;
	BoolContainer fixRegion;

	EnumContainerT<BackgroundGridType> bgGridType;
	ColorContainer bgGridColor;
	BoolContainer bgHScaleAuto;
	DoubleContainer bgHScaleInterval;
	DoubleContainer bgHSubScaleInterval;

	BoolContainer bgVScaleAuto;
	DoubleContainer bgVScaleInterval;
	DoubleContainer bgVSubScaleInterval;

	ColorContainer aspectRatioColor;
	QFontContainer aspectRatioFont;

	ColorContainer distanceMarkersColor;
	QFontContainer distanceMarkersFont;

	ColorContainer lbBankMarkersColor;
	QFontContainer lbBankMarkersFont;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGCONTAINER_H
