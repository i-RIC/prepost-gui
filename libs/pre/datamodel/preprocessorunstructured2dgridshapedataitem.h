#ifndef PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H
#define PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H

#include "preprocessorgridshapedataitem.h"

class PreProcessorUnstructured2dGridShapeDataItem : public PreProcessorGridShapeDataItem
{
private:
	static const int normalLineWidth = 1;

public:
	PreProcessorUnstructured2dGridShapeDataItem(PreProcessorDataItem* parent);
	~PreProcessorUnstructured2dGridShapeDataItem();

	void informGridUpdate() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(1);}

protected:
	void assignActorZValues(const ZDepthRange& range) override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void setupActors();
	void updateActorSetting() override;
};

#endif // PREPROCESSORUNSTRUCTURED2DGRIDSHAPEDATAITEM_H
