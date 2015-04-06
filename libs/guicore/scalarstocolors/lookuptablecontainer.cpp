#include "lookuptablecontainer.h"
#include <misc/xmlsupport.h>
#include <guibase/colortool.h>
#include <vtkColorTransferFunction.h>

#include <QDomNode>

LookupTableContainer::LookupTableContainer()
	: ScalarsToColorsContainer(0)
{
	m_vtkObj = vtkColorTransferFunction::New();
	m_vtkDarkObj = vtkColorTransferFunction::New();
	m_colorMap = ColorMapSettingWidget::Rainbow;
	m_autoRange = true;
	m_autoMin = 0;
	m_autoMax = 0;
	m_manualMin = 0;
	m_manualMax = 0;
		update();
}

LookupTableContainer::LookupTableContainer(ProjectDataItem* d)
	: ScalarsToColorsContainer(d)
{
	m_vtkObj = vtkColorTransferFunction::New();
	m_vtkDarkObj = vtkColorTransferFunction::New();
	m_colorMap = ColorMapSettingWidget::Rainbow;
	m_autoRange = true;
	m_autoMin = 0;
	m_autoMax = 0;
	m_manualMin = 0;
	m_manualMax = 0;
		update();
}

LookupTableContainer::LookupTableContainer(const LookupTableContainer& c)
	: ScalarsToColorsContainer(0)
{
	m_vtkObj = vtkColorTransferFunction::New();
	m_vtkDarkObj = vtkColorTransferFunction::New();

	m_colorMap = c.m_colorMap;
	m_customSetting = c.m_customSetting;
	m_autoRange = c.m_autoRange;
	m_autoMin = c.m_autoMin;
	m_autoMax = c.m_autoMax;
	m_manualMin = c.m_manualMin;
	m_manualMax = c.m_manualMax;
	update();
}

void LookupTableContainer::update()
{
	vtkColorTransferFunction* func = dynamic_cast<vtkColorTransferFunction*>(m_vtkObj);
	vtkColorTransferFunction* darkFunc = dynamic_cast<vtkColorTransferFunction*>(m_vtkDarkObj);
	double min;
	double max;
	if (m_autoRange){
		min = m_autoMin;
		max = m_autoMax;
//		table->SetRange(m_autoMin, m_autoMax);
//		darkTable->SetRange(m_autoMin, m_autoMax);
	}else{
		min = m_manualMin;
		max = m_manualMax;
//		table->SetRange(m_manualMin, m_manualMax);
//		darkTable->SetRange(m_manualMin, m_manualMax);
	}
	if (m_colorMap == ColorMapSettingWidget::Rainbow){
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
/*
		table->SetHueRange(0.67, 0.0);
		table->SetSaturationRange(1.0, 1.0);
		table->SetValueRange(1.0, 1.0);
		darkTable->SetHueRange(0.67, 0.0);
		darkTable->SetSaturationRange(1.0, 1.0);
		darkTable->SetValueRange(0.8, 0.8);
*/
	} else if (m_colorMap == ColorMapSettingWidget::Grayscale){
		func->RemoveAllPoints();
		func->SetColorSpaceToHSV();
		func->AddHSVPoint(min, 0, 0, 0);
		func->AddHSVPoint(max, 0, 0, 1);

		darkFunc->RemoveAllPoints();
		darkFunc->SetColorSpaceToHSV();
		darkFunc->AddHSVPoint(min, 0, 1, 0);
		darkFunc->AddHSVPoint(max, 0, 1, 0.8);
/*
		table->SetSaturationRange(0.0, 0.0);
		table->SetValueRange(0.0, 1.0);
		darkTable->SetSaturationRange(0.0, 0.0);
		darkTable->SetValueRange(0.0, 0.8);
*/
	} else if (m_colorMap == ColorMapSettingWidget::Custom){
		double c[3];

		func->RemoveAllPoints();
		func->SetColorSpaceToRGB();
		darkFunc->RemoveAllPoints();
		darkFunc->SetColorSpaceToRGB();

		switch (m_customSetting.type){
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
			for (int i = 0; i < m_customSetting.arbitrarySettings.count(); ++i){
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
	m_autoRange = iRIC::getBooleanAttribute(node, "autoRange");
	m_autoMin = iRIC::getDoubleAttribute(node, "autoMin");
	m_autoMax = iRIC::getDoubleAttribute(node, "autoMax");
	m_manualMin = iRIC::getDoubleAttribute(node, "manualMin");
	m_manualMax = iRIC::getDoubleAttribute(node, "manualMax");
	QString colormap = node.toElement().attribute("colorMap");
	if (colormap == "Rainbow"){
		m_colorMap = ColorMapSettingWidget::Rainbow;
	} else if (colormap == "Grayscale"){
		m_colorMap = ColorMapSettingWidget::Grayscale;
	} else if (colormap == "Custom"){
		m_colorMap = ColorMapSettingWidget::Custom;
		QString type = node.toElement().attribute("CustomType");
		if (type == "TwoColors"){
			m_customSetting.type = ColorMapSettingWidget::CustomSetting::tTwoColors;
		} else if (type == "ThreeColors"){
			m_customSetting.type = ColorMapSettingWidget::CustomSetting::tThreeColors;
		} else if (type == "Arbitrary"){
			m_customSetting.type = ColorMapSettingWidget::CustomSetting::tArbitrary;
		} else {
			m_customSetting.type = ColorMapSettingWidget::CustomSetting::tTwoColors;
		}
		m_customSetting.minColor = loadColorAttribute("minColor", node, Qt::blue);
		m_customSetting.midColor = loadColorAttribute("midColor", node, Qt::white);
		m_customSetting.maxColor = loadColorAttribute("maxColor", node, Qt::red);
		m_customSetting.midValue = iRIC::getDoubleAttribute(node, "midValue", 0);

		QDomNodeList cols = node.childNodes();
		m_customSetting.arbitrarySettings.clear();
		for (int i = 0; i < cols.count(); ++i){
			QDomNode colNode = cols.at(i);
			if (colNode.nodeName() != "Color"){continue;}
			double val = iRIC::getDoubleAttribute(colNode, "value");
			QColor col = iRIC::getColorAttribute(colNode, "color");
			ColorMapSettingWidget::CustomSetting::CustomColor cc;
			cc.value = val;
			cc.color = col;
			m_customSetting.arbitrarySettings.append(cc);
		}
	}
	update();
}

void LookupTableContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setBooleanAttribute(writer, "autoRange", m_autoRange);
	iRIC::setDoubleAttribute(writer, "autoMin", m_autoMin);
	iRIC::setDoubleAttribute(writer, "autoMax", m_autoMax);
	iRIC::setDoubleAttribute(writer, "manualMin", m_manualMin);
	iRIC::setDoubleAttribute(writer, "manualMax", m_manualMax);
	QString colormap;
	if (m_colorMap == ColorMapSettingWidget::Rainbow){
		colormap = "Rainbow";
	} else if (m_colorMap == ColorMapSettingWidget::Grayscale){
		colormap = "Grayscale";
	} else if (m_colorMap == ColorMapSettingWidget::Custom){
		colormap = "Custom";
	}
	writer.writeAttribute("colorMap", colormap);
	if (m_colorMap == ColorMapSettingWidget::Custom){
		if (m_customSetting.type == ColorMapSettingWidget::CustomSetting::tTwoColors){
			writer.writeAttribute("CustomType", "TwoColors");
		} else if (m_customSetting.type == ColorMapSettingWidget::CustomSetting::tThreeColors){
			writer.writeAttribute("CustomType", "ThreeColors");
		} else if (m_customSetting.type == ColorMapSettingWidget::CustomSetting::tArbitrary){
			writer.writeAttribute("CustomType", "Arbitrary");
		}
		writeColorAttribute("minColor", m_customSetting.minColor, writer);
		writeColorAttribute("maxColor", m_customSetting.maxColor, writer);
		writeColorAttribute("midColor", m_customSetting.midColor, writer);
		iRIC::setDoubleAttribute(writer, "midValue", m_customSetting.midValue);
		if (m_customSetting.type == ColorMapSettingWidget::CustomSetting::tArbitrary){
			for (int i = 0; i < m_customSetting.arbitrarySettings.count(); ++i){
				writer.writeStartElement("Color");
				iRIC::setDoubleAttribute(writer, "value", m_customSetting.arbitrarySettings.at(i).value);
				iRIC::setColorAttribute(writer, "color", m_customSetting.arbitrarySettings.at(i).color);
				writer.writeEndElement();
			}
		}
	}
}

void LookupTableContainer::getValueRange(double* min, double* max)
{
	if (m_autoRange){
		*min = m_autoMin;
		*max = m_autoMax;
	}else{
		*min = m_manualMin;
		*max = m_manualMax;
	}
}

void LookupTableContainer::setValueRange(double min, double max){
	double tmpmin = m_autoMin;
	double tmpmax = m_autoMax;
	m_autoMin = min;
	m_autoMax = max;
	if (m_autoMin == m_autoMax){
		double epsilon = 0.000001;
		m_autoMin -= epsilon;
		m_autoMax += epsilon;
	}
	if (m_autoRange){
		update();
		if (m_autoMin != tmpmin || m_autoMax != tmpmax) emit rangeChanged();
	}
}

LookupTableContainer& LookupTableContainer::operator=(const LookupTableContainer& c)
{
	m_colorMap = c.m_colorMap;
	m_customSetting = c.m_customSetting;
	m_autoRange = c.m_autoRange;
	m_autoMin = c.m_autoMin;
	m_autoMax = c.m_autoMax;
	m_manualMin = c.m_manualMin;
	m_manualMax = c.m_manualMax;
	update();
	return *this;
}

