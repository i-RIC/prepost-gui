#include "gridcomplexconditiongroup.h"
#include "gridcomplexconditiongrouprealeditwidget.h"
#include "ui_gridcomplexconditiongrouprealeditwidget.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "../../project/inputcond/inputconditioncontainerset.h"
#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../../solverdef/solverdefinitiontranslator.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QCheckBox>
#include <QDomElement>
#include <QVBoxLayout>

namespace {

GridComplexConditionGroupRealEditWidget::SubItem buildRealSubItem(const QDomElement& elem, const SolverDefinitionTranslator& t)
{
	auto name = elem.attribute("name");
	auto cap = t.translate(elem.attribute("caption"));
	return GridComplexConditionGroupRealEditWidget::SubItem {GridComplexConditionGroupRealEditWidget::SubItem::Real, name, "", cap};
}

std::vector<GridComplexConditionGroupRealEditWidget::SubItem> buildFunctionalSubItems(const QDomElement& elem, const SolverDefinitionTranslator& t)
{
	std::vector<GridComplexConditionGroupRealEditWidget::SubItem> ret;
	auto name = elem.attribute("name");
	auto cap = t.translate(elem.attribute("caption"));

	auto defNode = iRIC::getChildNode(elem, "Definition");
	auto nodes = defNode.childNodes();
	for (int i = 0; i < nodes.size(); ++i) {
		auto e = nodes.at(i).toElement();
		if (e.nodeName() != "Parameter" && e.nodeName() != "Value") {continue;}

		auto vType = e.attribute("valueType");
		if (vType != "real") {continue;}

		auto subName = e.attribute("name");
		auto subCapEng = e.attribute("caption");
		auto subCap = t.translate(subCapEng);

		if (subCapEng == "X" || subCapEng == "Y") {continue;}

		if (subName == "") {
			if (e.nodeName() == "Parameter") {
				subName = "Param";
			} else if (e.nodeName() == "Value") {
				subName = "Value";
			}
		}
		auto ccap = QString("%1 / %2").arg(cap).arg(subCap);
		ret.push_back(GridComplexConditionGroupRealEditWidget::SubItem {GridComplexConditionGroupRealEditWidget::SubItem::Functional, name, subName, ccap});
	}
	return ret;
}

void setupSubItemsRec(const QDomElement& elem, const SolverDefinitionTranslator& t, std::vector<GridComplexConditionGroupRealEditWidget::SubItem>* subItems)
{
	const auto nodes = elem.childNodes();
	for (int i = 0; i < nodes.size(); ++i) {
		const auto& node = nodes.at(i);
		if (node.nodeName() == "Item") {
			auto defNode = iRIC::getChildNode(node, "Definition");
			if (defNode.isNull()) {continue;}
			auto defElem = defNode.toElement();

			auto type = defElem.attribute("conditionType");
			auto valueType = defElem.attribute("valueType");

			if (type == "functional" || valueType == "functional") {
				auto items = buildFunctionalSubItems(node.toElement(), t);
				for (auto i : items) {
					subItems->push_back(i);
				}
			} else if (valueType == "real") {
				subItems->push_back(buildRealSubItem(node.toElement(), t));
			}
		} else {
			setupSubItemsRec(node.toElement(), t, subItems);
		}
	}
}

} // namespace

GridComplexConditionGroupRealEditWidget::GridComplexConditionGroupRealEditWidget(QWidget *parent, SolverDefinitionGridAttribute* att) :
	GridAttributeVariationEditWidget(parent, att),
	ui(new Ui::GridComplexConditionGroupRealEditWidget)
{
	ui->setupUi(this);
	ui->valueEdit->setValue(0);

	setupWidget();
}

GridComplexConditionGroupRealEditWidget::~GridComplexConditionGroupRealEditWidget()
{
	delete ui;
}

void GridComplexConditionGroupRealEditWidget::setMode(Mode mode)
{
	if (mode == Ratio) {
		ui->valueEdit->setValue(1.0);
	}
	GridAttributeVariationEditWidget::setMode(mode);
}

void GridComplexConditionGroupRealEditWidget::applyVariation(GridAttributeContainer* /*container*/, const std::vector<vtkIdType>& /*indices*/, vtkDataSetAttributes* /*atts*/, PreProcessorGridDataItemI* /*dItem*/)
{}

void GridComplexConditionGroupRealEditWidget::applyVariation(const std::vector<vtkIdType>& indices, const std::vector<GridComplexConditionGroup*>& groups)
{
	double v = ui->valueEdit->value();

	for (vtkIdType index : indices) {
		GridComplexConditionGroup* g = groups.at(index);
		auto cs = g->containerSet();
		for (int i = 0; i < m_subItems.size(); ++i) {
			const SubItem& item = m_subItems.at(i);
			const QCheckBox* cb = m_checkBoxes.at(i);
			if (! cb->isChecked()) {continue;}

			if (item.type == SubItem::Real) {
				auto& real = cs->real(iRIC::toStr(item.name));
				if (m_mode == Difference) {
					real.setValue(real.value() + v);
				} else if (m_mode == Ratio) {
					real.setValue(real.value() * v);
				}
			} else if (item.type == SubItem::Functional) {
				auto& func = cs->functional(iRIC::toStr(item.name));
				std::vector<double>* vals = nullptr;
				if (item.subName == "Param") {
					vals = &(func.x());
				} else if (item.subName == "Value") {
					vals = &(func.y());
				} else {
					vals = &(func.value(iRIC::toStr(item.subName)));
				}
				if (m_mode == Difference) {
					for (auto& val : *vals) {
						val += v;
					}
				} else if (m_mode == Ratio) {
					for (auto& val : *vals) {
						val *= v;
					}
				}
			}
		}
	}
}

void GridComplexConditionGroupRealEditWidget::getValueFromInnerWidget()
{
}

void GridComplexConditionGroupRealEditWidget::setupWidget()
{
	setupSubItems();

	auto l = new QVBoxLayout(this);
	for (const auto& subItem : m_subItems) {
		auto c = new QCheckBox(subItem.caption, this);
		m_checkBoxes.push_back(c);
		l->addWidget(c);
	}
	ui->targetsGroupBox->setLayout(l);
}

void GridComplexConditionGroupRealEditWidget::setupSubItems()
{
	auto att = dynamic_cast<SolverDefinitionGridComplexAttribute*> (gridAttribute());
	auto t = att->solverDefinition()->buildTranslator();
	auto elem = att->element();

	setupSubItemsRec(elem, t, &m_subItems);
}
