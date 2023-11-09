#ifndef POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <string>
#include <vector>

class Post2dWindowAttributeBrowserController;
class Post2dWindowCalculationResultDataItem;
class Post2dWindowNodeScalarGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowNodeScalarGroupTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeScalarGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarGroupTopDataItem();

	Post2dWindowAttributeBrowserController* attributeBrowserController() const;
	QAction* showAttributeBrowserAction() const;

	void updateZDepthRangeItemCount() override;
	void update();
	Post2dWindowCalculationResultDataItem* resultDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	bool hasTransparentPart() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	std::vector<std::string> scalarsDrawnInDiscreteMode() const;
	bool checkKmlExportCondition(const std::string& target);
	bool exportKMLHeader(QXmlStreamWriter& writer, const std::string& target);
	bool exportKMLFooter(QXmlStreamWriter& writer, const std::string& target);
	bool exportKMLForTimestep(QXmlStreamWriter& writer, const std::string& target, int index, double time, bool oneShot);

	bool checkShapeExportCondition(const std::string& target);
	bool exportContourFigureToShape(const std::string& target, const QString& filename, double time);

private slots:
	void showAttributeBrowser();

private:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QAction* m_showAttributeBrowserAction;
	class AttributeBrowserController;
	AttributeBrowserController* m_attributeBrowserController;

	friend class Post2dWindowNodeScalarGroupDataItem;
};

#endif // POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H
