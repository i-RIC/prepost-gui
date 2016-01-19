#ifndef GRIDATTRIBUTEDIMENSIONSCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONSCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionselectwidget.h"

#include <QObject>

#include <vector>

class GUICOREDLL_EXPORT GridAttributeDimensionsContainer : public QObject
{
	Q_OBJECT

public:
	GridAttributeDimensionsContainer(SolverDefinitionGridAttribute* conddef, QObject* parent);
	~GridAttributeDimensionsContainer();

	const std::vector<GridAttributeDimensionContainer*>& containers() const;
	std::vector<GridAttributeDimensionContainer*>& containers();

	const std::vector<GridAttributeDimensionSelectWidget*>& selectWidgets() const;
	std::vector<GridAttributeDimensionSelectWidget*>& selectWidgets();

	int currentIndex() const;
	void setCurrentIndex(int newIndex, bool noDraw = false);

	int calculateIndex(GridAttributeDimensionContainer* cont = nullptr, int index = 0, int size = 0) const;
	std::vector<int> calculateIndices(int index) const;
	int maxIndex() const;

signals:
	void currentIndexChanged();
	void currentIndexChanged(int oldIndex, int newIndex);
	void valuesChanged();

private slots:
	void handleIndexChange(bool noDraw);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/gridattributedimensionscontainer_impl.h"
#endif // _DEBUG

#endif // GRIDATTRIBUTEDIMENSIONSCONTAINER_H
