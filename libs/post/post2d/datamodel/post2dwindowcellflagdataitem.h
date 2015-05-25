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
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

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
