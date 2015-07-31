#include "lookuptablecontainer.h"

#include <guibase/colortool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>

LookupTableContainer::LookupTableContainer() :
	LookupTableContainer {nullptr}
{}

LookupTableContainer::LookupTableContainer(ProjectDataItem* d) :
	ScalarsToColorsContainer {d},
	m_autoRange {"autoRange", true},
	m_autoMin {"autoMin", 0},
	m_autoMax {"autoMax", 0},
	m_manualMin	{"manualMin", 0},
	m_manualMax {"manualMax", 0},
	m_colorMap {"colorMap", ColorMapSettingWidget::Rainbow}
{
	m_vtkObj = vtkColorTransferFunction::New();
	m_vtkDarkObj = vtkColorTransferFunction::New();

	update();
}

LookupTableContainer::LookupTableContainer(const LookupTableContainer& c) :
	LookupTableContainer {}
{
	m_colorMap = c.m_colorMap.value();
	m_customSetting = c.m_customSetting;
	m_autoRange = c.m_autoRange.value();
	m_autoMin = c.m_autoMin.value();
	m_autoMax = c.m_autoMax.value();
	m_manualMin = c.m_manualMin.value();
	m_manualMax = c.m_manualMax.value();

	update();
}

void LookupTableContainer::update()
{
	vtkColorTransferFunction* func = dynamic_cast<vtkColorTransferFunction*>(m_vtkObj);
	vtkColorTransferFunction* darkFunc = dynamic_cast<vtkColorTransferFunction*>(m_vtkDarkObj);
	double min;
	double max;
	if (m_autoRange) {
		min = m_autoMin;
		max = m_autoMax;
	} else {
		min = m_manualMin;
		max = m_manualMax;
	}
	if (m_colorMap == ColorMapSettingWidget::Rainbow) {
		func->RemoveAllPoints();
		func->SetColorSpaceToHSV();
		func->HSVWrapOff();
		func->AddHSVPoint(min, 0.66667, 1, 1);
		func->AddHSVPoint(max, 0,       1, 1);

		darkFunc->RemoveAllPoints();
		darkFunc->SetColorSpaceToHSV();
		darkFunc->HSVWrapOff();
		darkFunc->AddHSVPoint(min, 0.66667, 1, 0.8);
		darkFunc->AddHSVPoint(max, 0,       1, 0.8);
	} else if (m_colorMap == ColorMapSettingWidget::Grayscale) {
		func->RemoveAllPoints();
		func->SetColorSpaceToHSV();
		func->AddHSVPoint(min, 0, 0, 0);
		func->AddHSVPoint(max, 0, 0, 1);

		darkFunc->RemoveAllPoints();
		darkFunc->SetColorSpaceToHSV();
		darkFunc->AddHSVPoint(min, 0, 1, 0);
		darkFunc->AddHSVPoint(max, 0, 1, 0.8);
	} else if (m_colorMap == ColorMapSettingWidget::Custom) {
		double c[3];

		func->RemoveAllPoints();
		func->SetColorSpaceToRGB();
		darkFunc->RemoveAllPoints();
		darkFunc->SetColorSpaceToRGB();

		switch (ColorMapSettingWidget::CustomSetting::Type(m_customSetting.type)) {
		case ColorMapSettingWidget::CustomSetting::tTwoColors:
			iRIC::QColorToVTKColor(m_customSetting.minColor, c);
			func->AddRGBPoint(min, c[0], c[1], c[2]);
			darkFunc->AddRGBPoint(min, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
			iRIC::QColorToVTKColor(m_customSetting.maxColor, c);
			func->AddRGBPoint(max, c[0], c[1], c[2]);
			darkFunc->AddRGBPoint(max, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);

			break;
		case ColorMapSettingWidget::CustomSetting::tThreeColors:
			iRIC::QColorToVTKColor(m_customSetting.minColor, c);
			func->AddRGBPoint(min, c[0], c[1], c[2]);
			darkFunc->AddRGBPoint(min, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
			iRIC::QColorToVTKColor(m_customSetting.midColor, c);
			func->AddRGBPoint(m_customSetting.midValue, c[0], c[1], c[2]);
			darkFunc->AddRGBPoint(m_customSetting.midValue, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
			iRIC::QColorToVTKColor(m_customSetting.maxColor, c);
			func->AddRGBPoint(max, c[0], c[1], c[2]);
			darkFunc->AddRGBPoint(max, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);

			break;
		case ColorMapSettingWidget::CustomSetting::tArbitrary:
			for (int i = 0; i < m_customSetting.arbitrarySettings.count(); ++i) {
				double val = m_customSetting.arbitrarySettings.at(i).value;
				QColor col = m_customSetting.arbitrarySettings.at(i).color;
				iRIC::QColorToVTKColor(col, c);
				func->AddRGBPoint(val, c[0], c[1], c[2]);
				darkFunc->AddRGBPoint(val, c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
			}
			break;
		}
	}
	func->Modified();;
	darkFunc->Modified();
}

void LookupTableContainer::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_autoRange.load(node);
	m_autoMin.load(node);
	m_autoMax.load(node);
	m_manualMin.load(node);
	m_manualMax.load(node);
	m_colorMap.load(node);

	if (m_colorMap == ColorMapSettingWidget::Custom) {
		m_customSetting.load(node);
	}
	update();
}

void LookupTableContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_autoRange.save(writer);
	m_autoMin.save(writer);
	m_autoMax.save(writer);
	m_manualMin.save(writer);
	m_manualMax.save(writer);
	m_colorMap.save(writer);

	if (m_colorMap == ColorMapSettingWidget::Custom) {
		m_customSetting.save(writer);
	}
}

void LookupTableContainer::getValueRange(double* min, double* max)
{
	if (m_autoRange) {
		*min = m_autoMin;
		*max = m_autoMax;
	} else {
		*min = m_manualMin;
		*max = m_manualMax;
	}
}

void LookupTableContainer::setValueRange(double min, double max)
{
	double tmpmin = m_autoMin;
	double tmpmax = m_autoMax;
	m_autoMin = min;
	m_autoMax = max;
	if (m_autoMin == m_autoMax) {
		double epsilon = 0.000001;
		m_autoMin -= epsilon;
		m_autoMax += epsilon;
	}
	if (m_autoRange) {
		update();
		if (m_autoMin != tmpmin || m_autoMax != tmpmax) { emit rangeChanged(); }
	}
}

LookupTableContainer& LookupTableContainer::operator=(const LookupTableContainer& c)
{
	m_colorMap = c.m_colorMap.value();
	m_customSetting = c.m_customSetting;
	m_autoRange = c.m_autoRange.value();
	m_autoMin = c.m_autoMin.value();
	m_autoMax = c.m_autoMax.value();
	m_manualMin = c.m_manualMin.value();
	m_manualMax = c.m_manualMax.value();
	update();
	return *this;
}

