#ifndef GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H
#define GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H

#include "../../../guicore_global.h"
#include "gridrelatedconditiondimensioncontainer.h"
#include "gridrelatedconditiondimensionselectwidget.h"

#include <QObject>
#include <QList>

class GUICOREDLL_EXPORT GridRelatedConditionDimensionsContainer : public QObject
{
	Q_OBJECT

public:
	GridRelatedConditionDimensionsContainer(SolverDefinitionGridRelatedCondition* conddef, QObject* parent);
	~GridRelatedConditionDimensionsContainer();

	const QList<GridRelatedConditionDimensionContainer*>& containers() const {return m_containers;}
	QList<GridRelatedConditionDimensionContainer*> containers() {return m_containers;}
	const QList<GridRelatedConditionDimensionSelectWidget*>& selectWidgets() const {return m_selectWidgets;}
	QList<GridRelatedConditionDimensionSelectWidget*>& selectWidgets() {return m_selectWidgets;}

	void setCurrentIndex(int newIndex, bool noDraw = false);
	int currentIndex() const;
	QList<int> calculateIndices(int index);
	int maxIndex() const;
	int calculateIndex(GridRelatedConditionDimensionContainer* cont = 0, int index = 0, int size = 0) const;

signals:
	void currentIndexChanged();
	void currentIndexChanged(int oldIndex, int newIndex);
	void valuesChanged();

private slots:
	void handleIndexChange(bool noDraw);

private:
	int m_currentIndex;
	QList<GridRelatedConditionDimensionContainer*> m_containers;
	QList<GridRelatedConditionDimensionSelectWidget*> m_selectWidgets;

};

#endif // GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H
