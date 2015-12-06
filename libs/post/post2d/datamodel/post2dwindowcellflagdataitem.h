#ifndef POST2DWINDOWCELLFLAGDATAITEM_H
#define POST2DWINDOWCELLFLAGDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QColor>

#include <vtkSmartPointer.h>
#include <vtkExtractCells.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

class Post2dWindowCellFlagDataItem : public Post2dWindowDataItem
{

public:
	Post2dWindowCellFlagDataItem(const std::string& attname, int val, const QColor& col, const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowCellFlagDataItem();

	const std::string& attributeName() const;
	int value() const;
	const QColor& color() const;
	void setColor(const QColor& col);
	void setOpacity(int o);

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	void setupActors();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	std::string m_attributeName;
	int m_value;
	QColor m_color;

	vtkSmartPointer<vtkExtractCells> m_extractCells;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;
};

#endif // POST2DWINDOWCELLFLAGDATAITEM_H
