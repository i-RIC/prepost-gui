#include "../pre/gridcond/base/gridattributeeditdialog.h"
#include "../pre/gridcond/base/gridattributevariationeditdialog.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "../scalarstocolors/colortransferfunctioneditwidget.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../scalarstocolors/lookuptableeditwidget.h"
#include "../scalarstocolors/scalarstocolorseditdialog.h"
#include "solverdefinitiongridattribute.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiontranslator.h"

#include <misc/xmlsupport.h>

#include <QSettings>

SolverDefinitionGridAttribute::~SolverDefinitionGridAttribute()
{
	for (SolverDefinitionGridAttributeDimension* dim : m_dimensions) {
		delete dim;
	}
}

void SolverDefinitionGridAttribute::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = node.attribute("name");
	m_englishCaption = node.attribute("caption");
	m_caption = translator.translate(node.attribute("caption"));
	QDomNode defNode = iRIC::getChildNode(node, "Definition");
	if (! defNode.isNull()) {
		QDomElement e = defNode.toElement();
		m_variantDefaultValue = e.attribute("default", "");
		m_variantMaximumValue = e.attribute("max", "");
		m_variantMaximumValue = e.attribute("min", "");
		QDomNodeList children = e.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomNode childNode = children.at(i);
			if (childNode.nodeName() == "Dimension") {
				// add dimension
				SolverDefinitionGridAttributeDimension* dim = SolverDefinitionGridAttributeDimensionCreator::create(childNode.toElement(), translator, this);
				m_dimensions.append(dim);
			}
		}
	}
}

GridAttributeContainer* SolverDefinitionGridAttribute::container(Grid* grid)
{
	return buildContainer(grid);
}

GridAttributeEditDialog* SolverDefinitionGridAttribute::editDialog(QWidget* parent)
{
	GridAttributeEditDialog* dialog = new GridAttributeEditDialog(parent);
	GridAttributeEditWidget* widget = editWidget(0);
	dialog->setWidget(widget);
	return dialog;
}

GridAttributeVariationEditDialog* SolverDefinitionGridAttribute::variationEditDialog(QWidget* parent)
{
	GridAttributeVariationEditDialog* dialog = new GridAttributeVariationEditDialog(parent);
	GridAttributeVariationEditWidget* widget = variationEditWidget(0);
	dialog->setWidget(widget);
	return dialog;
}

ScalarsToColorsContainer* SolverDefinitionGridAttribute::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createLookupTableContainer(d);
}

ColorTransferFunctionContainer* SolverDefinitionGridAttribute::createColorTransferFunctionContainer(ProjectDataItem* d)
{
	return new ColorTransferFunctionContainer(d);
}

LookupTableContainer* SolverDefinitionGridAttribute::createLookupTableContainer(ProjectDataItem* d)
{
	LookupTableContainer* container = new LookupTableContainer(d);
	QSettings setting;
	int cm = setting.value("graphics/colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	container->setColorMap(cmVal);
	return container;
}

ScalarsToColorsEditDialog* SolverDefinitionGridAttribute::createScalarsToColorsEditDialog(QWidget* parent)
{
	ScalarsToColorsEditDialog* dialog = new ScalarsToColorsEditDialog(parent);
	ScalarsToColorsEditWidget* widget = createScalarsToColorsEditWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createLookupTableEditWidget(parent);
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createColorTransferFunctionEditWidget(QWidget* parent)
{
	return new ColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createLookupTableEditWidget(QWidget* parent)
{
	LookupTableEditWidget* widget = new LookupTableEditWidget(parent);
	widget->hideFillSettings();
	widget->hideDivisionNumber();
	return widget;
}
