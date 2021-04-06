#ifndef LOOKUPTABLECONTAINER_H
#define LOOKUPTABLECONTAINER_H

#include "../guicore_global.h"
#include "scalarstocolorscontainer.h"
#include <guibase/colormap/colormapcustomsetting.h>
#include <guibase/colormap/colormapsettingwidget.h>
#include <misc/doublecontainer.h>
#include <misc/boolcontainer.h>
#include <misc/enumcontainert.h>


class GUICOREDLL_EXPORT LookupTableContainer : public ScalarsToColorsContainer
{

public:
	LookupTableContainer();
	explicit LookupTableContainer(ProjectDataItem* d);
	LookupTableContainer(const LookupTableContainer& c);

	LookupTableContainer& operator=(const LookupTableContainer& c);

	void update() override;

	void getValueRange(double* min, double* max) const override;
	virtual void setValueRange(double min, double max) override;

	ColorMapSettingWidget::ColorMap colorMap() const;
	void setColorMap(ColorMapSettingWidget::ColorMap cm);

	const ColorMapCustomSetting& customSetting() const;
	void setCustomSetting(const ColorMapCustomSetting& cs);

	bool autoRange() const;
	void setAutoRange(bool ar);

	double autoMin() const;
	double autoMax() const;

	double manualMin() const;
	void setManualMin(double min);

	double manualMax() const;
	void setManualMax(double max);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	BoolContainer m_autoRange;
	DoubleContainer m_autoMin;
	DoubleContainer m_autoMax;

	DoubleContainer m_manualMin;
	DoubleContainer m_manualMax;

	EnumContainerT<ColorMapSettingWidget::ColorMap> m_colorMap;

	ColorMapCustomSetting m_customSetting;
};

#endif // LOOKUPTABLECONTAINER_H
