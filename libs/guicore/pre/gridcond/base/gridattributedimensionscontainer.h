#ifndef GRIDATTRIBUTEDIMENSIONSCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONSCONTAINER_H

#include "../../../guicore_global.h"
#include "../../../project/projectdataitem.h"
#include "gridattributedimensioncontainer.h"
#include "gridattributedimensionselectwidget.h"

#include <QObject>

#include <vector>

class ProjectData;

class GUICOREDLL_EXPORT GridAttributeDimensionsContainer : public ProjectDataItem
{
	Q_OBJECT

public:
	GridAttributeDimensionsContainer(SolverDefinitionGridAttribute* conddef, ProjectDataItem* parent);
	~GridAttributeDimensionsContainer();

	const std::vector<GridAttributeDimensionContainer*>& containers() const;
	std::vector<GridAttributeDimensionContainer*>& containers();

	const std::vector<GridAttributeDimensionSelectWidget*>& selectWidgets() const;
	std::vector<GridAttributeDimensionSelectWidget*>& selectWidgets();

	void clear();

	int currentIndex() const;
	void setCurrentIndex(int newIndex, bool noDraw = false);

	int calculateIndex(GridAttributeDimensionContainer* cont = nullptr, int index = 0, int size = 0) const;
	std::vector<int> calculateIndices(int index) const;
	int maxIndex() const;

	ProjectData* projectData();

signals:
	void currentIndexChanged();
	void currentIndexChanged(int oldIndex, int newIndex);
	void valuesChanged();

private slots:
	void handleIndexChange(bool noDraw);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/gridattributedimensionscontainer_impl.h"
#endif // _DEBUG

#endif // GRIDATTRIBUTEDIMENSIONSCONTAINER_H
