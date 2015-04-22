#ifndef GRIDRELATEDCONDITIONDIMENSIONSELECTWIDGET_H
#define GRIDRELATEDCONDITIONDIMENSIONSELECTWIDGET_H

#include <QWidget>

class GridRelatedConditionDimensionContainer;

class GridRelatedConditionDimensionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	GridRelatedConditionDimensionSelectWidget(GridRelatedConditionDimensionContainer* container, QWidget* parent = nullptr);
	~GridRelatedConditionDimensionSelectWidget();
	GridRelatedConditionDimensionContainer* container() const {return m_container;}

protected slots:
	void applyValues();
	void setCurrentIndex(int newIndex);

signals:
	void currentIndexChanged(int newIndex);

protected:
	virtual void doApplyValues() = 0;
	virtual void doSetCurrentIndex(int newIndex) = 0;
	GridRelatedConditionDimensionContainer* m_container;
};

#endif // GRIDRELATEDCONDITIONDIMENSIONSELECTWIDGET_H
