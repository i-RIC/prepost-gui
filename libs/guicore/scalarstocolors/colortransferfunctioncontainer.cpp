#include "colortransferfunctioncontainer.h"
#include <guibase/colortool.h>

#include <vtkColorTransferFunction.h>

ColorTransferFunctionContainer::ColorTransferFunctionContainer(ProjectDataItem* d)
	: ScalarsToColorsContainer(d)
{
	m_vtkObj = vtkColorTransferFunction::New();
	m_vtkDarkObj = vtkColorTransferFunction::New();
}

void ColorTransferFunctionContainer::update()
{
	vtkColorTransferFunction* ctf = dynamic_cast<vtkColorTransferFunction*>(m_vtkObj);
	vtkColorTransferFunction* ctfDark = dynamic_cast<vtkColorTransferFunction*>(m_vtkDarkObj);
	ctf->RemoveAllPoints();
	ctfDark->RemoveAllPoints();
	double c[3];
	for (auto it = m_colors.begin(); it != m_colors.end(); ++it){
		iRIC::QColorToVTKColor(it.value(), c);
		ctf->AddRGBPoint(it.key(), c[0], c[1], c[2]);
		ctfDark->AddRGBPoint(it.key(), c[0] * 0.8, c[1] * 0.8, c[2] * 0.8);
	}
}

void ColorTransferFunctionContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void ColorTransferFunctionContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
