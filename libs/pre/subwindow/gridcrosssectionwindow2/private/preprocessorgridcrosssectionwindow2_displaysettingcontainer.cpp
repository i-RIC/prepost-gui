#include "preprocessorgridcrosssectionwindow2_impl.h"
#include <guibase/widget/realnumbereditwidget.h>

PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::DisplaySettingContainer() :
	CompositeContainer({&gridVisible, &scaleVisible, &markersVisible, &aspectRatioVisible,
										 &enableAutoRescale, &fixAspectRatio, &fixRegion,
										 &bgGridType, &bgGridColor,
										 &bgHScaleAuto, &bgHScaleInterval, &bgHSubScaleInterval,
										 &bgVScaleAuto, &bgVScaleInterval, &bgVSubScaleInterval,
										 &aspectRatioColor, &aspectRatioFont,
										 &distanceMarkersColor, &distanceMarkersFont,
										 &lbBankMarkersColor, &lbBankMarkersFont}),
	gridVisible {"gridVisible", true},
	scaleVisible {"scaleVisible", true},
	markersVisible {"markersVisible", true},
	aspectRatioVisible {"aspectRatioVisible", true},
	enableAutoRescale {"enableAutoRescale", true},
	fixAspectRatio {"fixAspectRatio", false},
	fixRegion {"fixRegion", false},
	bgGridType {"bgGridType", BackgroundGridType::Lines},
	bgGridColor {"bgGridColor", Qt::gray},
	bgHScaleAuto {"bgHScaleAuto", true},
	bgHScaleInterval {"bgHScaleInterval", 10},
	bgHSubScaleInterval {"bgHSubScaleInterval", 2},
	bgVScaleAuto {"bgVScaleAuto", true},
	bgVScaleInterval {"bgVScaleInterval", 10},
	bgVSubScaleInterval {"bgVSubScaleInterval", 2},
	aspectRatioColor {"aspectRatioColor", Qt::darkGray},
	aspectRatioFont {"aspectRatioFont", QFont("Times", 10)},
	distanceMarkersColor {"distanceMarkersColor", QColor(160, 160, 50)},
	distanceMarkersFont {"distanceMarkersFont", QFont("Helvetica", 10)},
	lbBankMarkersColor {"lbBankMarkersColor", QColor(160, 160, 50)},
	lbBankMarkersFont {"lbBankMarkersFont", QFont("Helvetica", 10)}
{}

PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::DisplaySettingContainer(const DisplaySettingContainer& setting) :
	DisplaySettingContainer {}
{
	copyValue(setting);
}

PreProcessorGridCrosssectionWindow2::DisplaySettingContainer& PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::operator=(const DisplaySettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const DisplaySettingContainer&> (c));
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::applyFromWidgets(PreProcessorGridCrosssectionWindow2::Impl* impl)
{
	gridVisible = impl->m_gridDisplayCheckBox->isChecked();
	scaleVisible = impl->m_scaleDisplayCheckBox->isChecked();
	markersVisible = impl->m_markersDisplayCheckBox->isChecked();
	aspectRatioVisible = impl->m_markersDisplayCheckBox->isChecked();

	enableAutoRescale = impl->m_autoRescaleCheckBox->isChecked();
	fixAspectRatio = impl->m_fixAspectRatioCheckBox->isChecked();
	fixRegion = impl->m_fixRegionCheckBox->isChecked();
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingContainer::applyToWidgets(PreProcessorGridCrosssectionWindow2::Impl* impl)
{
	impl->m_gridDisplayCheckBox->setChecked(gridVisible);
	impl->m_scaleDisplayCheckBox->setChecked(scaleVisible);
	impl->m_markersDisplayCheckBox->setChecked(markersVisible);
	impl->m_markersDisplayCheckBox->setChecked(aspectRatioVisible);

	impl->m_autoRescaleCheckBox->setChecked(enableAutoRescale);
	impl->m_fixAspectRatioCheckBox->setChecked(fixAspectRatio);
	impl->m_fixRegionCheckBox->setChecked(fixRegion);
}
