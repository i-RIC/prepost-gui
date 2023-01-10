#ifndef GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H
#define GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H

#include <QWidget>

class GridAttributeDimensionContainer;
class ProjectMainFile;

class GridAttributeDimensionSelectWidget : public QWidget
{
	Q_OBJECT

public:
	GridAttributeDimensionSelectWidget(GridAttributeDimensionContainer* container, QWidget* parent = nullptr);
	~GridAttributeDimensionSelectWidget();

	GridAttributeDimensionContainer* container() const;

	void setProjectMainFile(ProjectMainFile* mainFile);

protected slots:
	void applyValues();
	void setCurrentIndex(int newIndex);

signals:
	void currentIndexChanged(int newIndex);

protected:
	ProjectMainFile* projectMainFile() const;

private:
	virtual void doApplyValues() = 0;
	virtual void doSetCurrentIndex(int newIndex) = 0;

	GridAttributeDimensionContainer* m_container;
	ProjectMainFile* m_projectMainFile;
};

#endif // GRIDATTRIBUTEDIMENSIONSELECTWIDGET_H
