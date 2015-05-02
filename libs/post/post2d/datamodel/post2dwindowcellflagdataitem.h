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
	Post2dWindowCellFlagDataItem(const QString& attname, int val, const QColor& col, const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowCellFlagDataItem();
	const QString& attributeName() const {return m_attributeName;}
	int value() const {return m_value;}
	const QColor& color() const {return m_color;}
	void setColor(const QColor& col);
	void setOpacity(int o);
	void updateZDepthRangeItemCount();
	void assignActionZValues(const ZDepthRange& range);
	void update();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	void setupActors();

	QString m_attributeName;
	int m_value;
	QColor m_color;

	vtkSmartPointer<vtkExtractCells> m_extractCells;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;
};

#endif // POST2DWINDOWCELLFLAGDATAITEM_H
