#ifndef GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H
#define GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionselectwidget.h"

#include <QObject>
#include <QList>

class GUICOREDLL_EXPORT GridAttributeDimensionsContainer : public QObject
{
	Q_OBJECT

public:
	GridAttributeDimensionsContainer(SolverDefinitionGridAttribute* conddef, QObject* parent);
	~GridAttributeDimensionsContainer();

	const QList<GridAttributeDimensionContainer*>& containers() const {return m_containers;}
	QList<GridAttributeDimensionContainer*> containers() {return m_containers;}
	const QList<GridAttributeDimensionSelectWidget*>& selectWidgets() const {return m_selectWidgets;}
	QList<GridAttributeDimensionSelectWidget*>& selectWidgets() {return m_selectWidgets;}

	void setCurrentIndex(int newIndex, bool noDraw = false);
	int currentIndex() const;
	QList<int> calculateIndices(int index);
	int maxIndex() const;
	int calculateIndex(GridAttributeDimensionContainer* cont = nullptr, int index = 0, int size = 0) const;

signals:
	void currentIndexChanged();
	void currentIndexChanged(int oldIndex, int newIndex);
	void valuesChanged();

private slots:
	void handleIndexChange(bool noDraw);

private:
	int m_currentIndex;
	QList<GridAttributeDimensionContainer*> m_containers;
	QList<GridAttributeDimensionSelectWidget*> m_selectWidgets;

};

#endif // GRIDRELATEDCONDITIONDIMENSIONSCONTAINER_H
