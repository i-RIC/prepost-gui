#ifndef PREPROCESSORBCDATAITEM_H
#define PREPROCESSORBCDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/edge.h>

#include <QString>
#include <QColor>
#include <QSet>

class Grid;
class OpacityContainer;
class SolverDefinition;
class SolverDefinitionBoundaryCondition;
class VTKGraphicsView;

class QAction;

namespace iRICLib {
	class H5CgnsZone;
} // namespace iRICLib

class PreProcessorBCDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorBCDataItem(SolverDefinition* def, SolverDefinitionBoundaryCondition* cond, GraphicsWindowDataItem* parent, bool hideSetting = false);
	~PreProcessorBCDataItem() override;

	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone);
	int saveToCgnsFile(iRICLib::H5CgnsZone* zone);
	int importFromCgnsFile(const iRICLib::H5CgnsZone& zone);

	void handleStandardItemDoubleClicked() override;
	void handleStandardItemChange() override;

	void setName(const QString& name);

	void setProjectNumber(int num);
	int projectNumber() const;

	void setCgnsNumber(int num);
	int cgnsNumber() const;

	void setColor(const QColor& color);
	QColor color() const;

	OpacityContainer opacity() const;
	SolverDefinitionBoundaryCondition* condition() const;
	bool isValid() const;
	std::string uniqueName() const;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;
	void clearPoints();
	void assignIndices(const QSet<vtkIdType>& vertices);
	bool isCustomModified() const;
	QString caption() const;
	void setMapped(bool mapped);
	bool mapped() const;

	void updateNameActorSettings();

	QAction* editAction() const;
	QAction* deleteAction() const;
	QAction* assignAction() const;
	QAction* releaseAction() const;
	bool hideSetting() const;

public slots:
	bool showDialog();

private slots:
	void setModified(bool modified = true) override;
	void assignSelectedElements();
	void releaseSelectedElements();

signals:
	void itemUpdated();

private:
	void setupActors();
	void updateActorSettings();
	void updateElements();
	int buildNumber() const;
	void setupIndicesAndEdges(const std::vector<int> indices);
	void buildIndices(std::vector<int>* indices);

	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doApplyOffset(double x, double y) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/preprocessorbcdataitem_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORBCDATAITEM_H
