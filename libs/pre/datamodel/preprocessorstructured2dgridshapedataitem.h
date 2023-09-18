#ifndef PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H
#define PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H

#include "preprocessorgridshapedataitem.h"

class PreProcessorStructured2dGridShapeDataItem : public PreProcessorGridShapeDataItem
{

private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;

public:
	PreProcessorStructured2dGridShapeDataItem(PreProcessorDataItem* parent);
	~PreProcessorStructured2dGridShapeDataItem();

	void informGridUpdate() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;

private:
	void assignActorZValues(const ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void setupActors();
	void updateActorSetting() override;
};

#endif // PREPROCESSORSTRUCTURED2DGRIDSHAPEDATAITEM_H
