#ifndef LOOKUPTABLECONTAINER_H
#define LOOKUPTABLECONTAINER_H

#include "../guicore_global.h"
#include "scalarstocolorscontainer.h"
#include <guibase/colormapsettingwidget.h>

class GUICOREDLL_EXPORT LookupTableContainer : public ScalarsToColorsContainer
{
public:
	LookupTableContainer();
	LookupTableContainer(ProjectDataItem* d);
	LookupTableContainer(const LookupTableContainer& c);
	void update();
	virtual void setValueRange(double min, double max);
	void getValueRange(double* min, double* max);
	ColorMapSettingWidget::ColorMap colorMap() {return m_colorMap;}
	void setColorMap(ColorMapSettingWidget::ColorMap cm) {
		m_colorMap = cm;
	}
	const ColorMapSettingWidget::CustomSetting& customSetting() {return m_customSetting;}
	void setCustomSetting(ColorMapSettingWidget::CustomSetting cs) {
		m_customSetting = cs;
	}
	bool autoRange() {return m_autoRange;}
	void setAutoRange(bool ar) {m_autoRange = ar;}
	double autoMin() {return m_autoMin;}
	double autoMax() {return m_autoMax;}
	double manualMin() {return m_manualMin;}
	void setManualMin(double min) {m_manualMin = min;}
	double manualMax() {return m_manualMax;}
	void setManualMax(double max) {m_manualMax = max;}
	LookupTableContainer& operator=(const LookupTableContainer& c);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

	bool m_autoRange;
	double m_autoMin;
	double m_autoMax;
	double m_manualMin;
	double m_manualMax;
	ColorMapSettingWidget::ColorMap m_colorMap;
	ColorMapSettingWidget::CustomSetting m_customSetting;
};

#endif // LOOKUPTABLECONTAINER_H
