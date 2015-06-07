#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include <guicore/project/projectdataitem.h>

class PreProcessorNormal15DGridWithCrossSectionShapeDataItem;
class Structured15DGridWithCrossSectionCrossSectionWindow;

class Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem : public ProjectDataItem
{

public:
	Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem(PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, QWidget* parentWindow);
	~Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem();
	void informWindowClose();
	void requestWindowClose();
	Structured15DGridWithCrossSectionCrossSectionWindow* window() {
		return m_window;
	}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

protected:
	Structured15DGridWithCrossSectionCrossSectionWindow* m_window;
	/*

	   public:
	   friend class PostProcessorWindow;
	 */
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOWPROJECTDATAITEM_H
