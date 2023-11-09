#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowAbstractCellScalarGroupDataItem;
class Post2dWindowAttributeBrowserController;
class Post2dWindowZoneDataItem;
class Post2dWindowCalculationResultDataItem;
class vtkPointSetExtended;

class Post2dWindowAbstractCellScalarGroupTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowAbstractCellScalarGroupTopDataItem(const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowAbstractCellScalarGroupTopDataItem();

	void setupChildren();
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

private slots:
	void showAttributeBrowser();

private:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkCellData* cellData() const;
	virtual void getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const = 0;
	virtual vtkPointSetExtended* data() const = 0;
	virtual Post2dWindowAbstractCellScalarGroupDataItem* createChild(const std::string& target) = 0;

	QAction* m_showAttributeBrowserAction;
	class AttributeBrowserController;
	AttributeBrowserController* m_attributeBrowserController;

	friend class Post2dWindowCellScalarGroupDataItem;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_H
