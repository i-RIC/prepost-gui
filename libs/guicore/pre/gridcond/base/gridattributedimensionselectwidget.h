#ifndef GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H
#define GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H

#include <QWidget>

class GridAttributeDimensionContainer;

class GridAttributeDimensionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeDimensionSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent = nullptr);
	~GridAttributeDimensionSelectWidget();
	GridAttributeDimensionContainer* container() const {return m_container;}

protected slots:
	void applyValues();
	void setCurrentIndex(int newIndex);

signals:
	void currentIndexChanged(int newIndex);

protected:
	virtual void doApplyValues() = 0;
	virtual void doSetCurrentIndex(int newIndex) = 0;
	GridAttributeDimensionContainer* m_container;
};

#endif // GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H
