#include "../pre/gridcond/base/gridrelatedconditioneditdialog.h"
#include "../pre/gridcond/base/gridrelatedconditionvariationeditdialog.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "../scalarstocolors/colortransferfunctioneditwidget.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../scalarstocolors/lookuptableeditwidget.h"
#include "../scalarstocolors/scalarstocolorseditdialog.h"
#include "solverdefinitiongridrelatedcondition.h"
#include "solverdefinitiongridrelatedconditiondimensioncreator.h"
#include "solverdefinitiontranslator.h"

#include <misc/xmlsupport.h>

#include <QSettings>

SolverDefinitionGridRelatedCondition::~SolverDefinitionGridRelatedCondition()
{
	for (int i = 0; i < m_dimensions.size(); ++i) {
		delete m_dimensions.at(i);
	}
}

void SolverDefinitionGridRelatedCondition::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
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
				SolverDefinitionGridRelatedConditionDimension* dim = SolverDefinitionGridRelatedConditionDimensionCreator::create(childNode.toElement(), translator, this);
				m_dimensions.append(dim);
			}
		}
	}
}

GridRelatedConditionContainer* SolverDefinitionGridRelatedCondition::container(Grid* grid)
{
	return buildContainer(grid);
}

GridRelatedConditionEditDialog* SolverDefinitionGridRelatedCondition::editDialog(QWidget* parent)
{
	GridRelatedConditionEditDialog* dialog = new GridRelatedConditionEditDialog(parent);
	GridRelatedConditionEditWidget* widget = editWidget(0);
	dialog->setWidget(widget);
	return dialog;
}

GridRelatedConditionVariationEditDialog* SolverDefinitionGridRelatedCondition::variationEditDialog(QWidget* parent)
{
	GridRelatedConditionVariationEditDialog* dialog = new GridRelatedConditionVariationEditDialog(parent);
	GridRelatedConditionVariationEditWidget* widget = variationEditWidget(0);
	dialog->setWidget(widget);
	return dialog;
}

ScalarsToColorsContainer* SolverDefinitionGridRelatedCondition::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createLookupTableContainer(d);
}

ColorTransferFunctionContainer* SolverDefinitionGridRelatedCondition::createColorTransferFunctionContainer(ProjectDataItem* d)
{
	return new ColorTransferFunctionContainer(d);
}

LookupTableContainer* SolverDefinitionGridRelatedCondition::createLookupTableContainer(ProjectDataItem* d)
{
	LookupTableContainer* container = new LookupTableContainer(d);
	QSettings setting;
	int cm = setting.value("graphics/colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	container->setColorMap(cmVal);
	return container;
}

ScalarsToColorsEditDialog* SolverDefinitionGridRelatedCondition::createScalarsToColorsEditDialog(QWidget* parent)
{
	ScalarsToColorsEditDialog* dialog = new ScalarsToColorsEditDialog(parent);
	ScalarsToColorsEditWidget* widget = createScalarsToColorsEditWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

ScalarsToColorsEditWidget* SolverDefinitionGridRelatedCondition::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createLookupTableEditWidget(parent);
}

ScalarsToColorsEditWidget* SolverDefinitionGridRelatedCondition::createColorTransferFunctionEditWidget(QWidget* parent)
{
	return new ColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsEditWidget* SolverDefinitionGridRelatedCondition::createLookupTableEditWidget(QWidget* parent)
{
	LookupTableEditWidget* widget = new LookupTableEditWidget(parent);
	widget->hideFillSettings();
	widget->hideDivisionNumber();
	return widget;
}
