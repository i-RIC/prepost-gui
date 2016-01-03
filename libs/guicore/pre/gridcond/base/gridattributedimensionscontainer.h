#ifndef GRIDATTRIBUTEDIMENSIONSCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONSCONTAINER_H

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

	const QList<GridAttributeDimensionContainer*>& containers() const;
	QList<GridAttributeDimensionContainer*> containers();

	const QList<GridAttributeDimensionSelectWidget*>& selectWidgets() const;
	QList<GridAttributeDimensionSelectWidget*>& selectWidgets();

	int currentIndex() const;
	void setCurrentIndex(int newIndex, bool noDraw = false);

	int calculateIndex(GridAttributeDimensionContainer* cont = nullptr, int index = 0, int size = 0) const;
	QList<int> calculateIndices(int index);
	int maxIndex() const;

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

#endif // GRIDATTRIBUTEDIMENSIONSCONTAINER_H
