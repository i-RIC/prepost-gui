#include "geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.h"
#include "geodatapointmap_polygonsmanager_mappingsetting.h"
#include "ui_geodatapointmap_polygonsmanager_attributeformappingcondition_editwidget.h"

namespace {

const double DEFAULT_RADIUS = 10;

} // namespace

GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::EditWidget(QWidget *parent, SolverDefinitionGridAttribute* att) :
	GridAttributeEditWidget(parent, att),
	ui(new Ui::GeoDataPointmap_PolygonsManager_AttributeForMappingCondition_EditWidget)
{
	ui->setupUi(this);
}

GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::~EditWidget()
{
	delete ui;
}

QVariant GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::variantValue() const
{
	MappingSetting s;
	if (ui->tinRadioButton->isChecked()) {
		s.type = MappingSetting::Type::TIN;
	} else if (ui->averageRadioButton->isChecked()) {
		s.type = MappingSetting::Type::WeightedAverage;
	} else if (ui->maxRadioButton->isChecked()) {
		s.type = MappingSetting::Type::Maximum;
	} else if (ui->minRadioButton->isChecked()) {
		s.type = MappingSetting::Type::Minimum;
	}
	s.radius = ui->radiusSpinBox->value();
	s.exponent = ui->exponentSpinBox->value();

	return s.toVariant();
}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::setVariantValue(const QVariant& v)
{
	MappingSetting s;
	s.fromVariant(v);
	if (s.type == MappingSetting::Type::TIN) {
		ui->tinRadioButton->setChecked(true);
	} else if (s.type == MappingSetting::Type::WeightedAverage) {
		ui->averageRadioButton->setChecked(true);
	} else if (s.type == MappingSetting::Type::Maximum) {
		ui->maxRadioButton->setChecked(true);
	} else if (s.type == MappingSetting::Type::Minimum) {
		ui->minRadioButton->setChecked(true);
	}
	ui->radiusSpinBox->setValue(s.radius);
	ui->exponentSpinBox->setValue(s.exponent);
}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::scanAndSetDefault(GridAttributeContainer* /*container*/, const std::vector<vtkIdType>& /*indices*/)
{}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::applyValue(GridAttributeContainer* /*container*/, const std::vector<vtkIdType>& /*indices*/, vtkDataSetAttributes* /*atts*/, PreProcessorGridDataItemInterface* /*dItem*/)
{}

QSize GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::sizeHint() const
{
	return QWidget::sizeHint();
}

QSize GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::minimumSizeHint() const
{
	return QWidget::minimumSizeHint();
}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::getValueFromInnerWidget() const
{}

void GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::setupWidget()
{
	// set default setting;
	MappingSetting s;
	setVariantValue(s.toVariant());
}

QWidget* GeoDataPointmap::PolygonsManager::AttributeForMappingCondition::EditWidget::editWidget() const
{
	return const_cast<EditWidget*> (this);
}
