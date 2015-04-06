#ifndef PREPROCESSORGRIDTYPEDATAITEM_H
#define PREPROCESSORGRIDTYPEDATAITEM_H

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>

#include <QList>
#include <QMap>

class QAction;
class SolverDefinitionGridType;
class PreProcessorRawDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;
class ScalarsToColorsContainer;

class PreProcessorGridTypeDataItem : public PreProcessorGridTypeDataItemInterface
{
	Q_OBJECT
public:
	PreProcessorGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent);
	~PreProcessorGridTypeDataItem();
	const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const {return m_conditions;}
	const QString& name() const;
	PreProcessorRawDataTopDataItemInterface* rawdataTop() const {return m_rawdataTop;}
	PreProcessorGridAndGridCreatingConditionDataItemInterface* condition(const QString& name) const;
	SolverDefinitionGridType* gridType() const {return m_gridType;}
	bool isChildDeletable(PreProcessorGridAndGridCreatingConditionDataItemInterface* child) const;
	void addCustomMenuItems(QMenu* menu);
	bool isChildCaptionAvailable(const QString& caption);
	ScalarsToColorsContainer* scalarsToColors(const QString& attName) const {return m_scalarsToColors.value(attName, 0);}
	QAction* addNewGridAction(){return m_addNewGridAction;}
	bool gridEdited() const;
	void toggleGridEditFlag();

public slots:
	void addNewCondition();
	void changeValueRange(const QString& name);

protected:
	void unregisterChild(GraphicsWindowDataItem* child);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void assignActionZValues(const ZDepthRange& range);

private:
	void updateNewGridActionStatus();
	void setupScalarsToColors(SolverDefinitionGridType* type);
	const QString nextChildCaption();
	const QString nextChildZonename();
	SolverDefinitionGridType* m_gridType;
	PreProcessorRawDataTopDataItemInterface* m_rawdataTop;
	QMap<QString, ScalarsToColorsContainer*> m_scalarsToColors;
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> m_conditions;
	/// Action to add new condition.
	QAction* m_addNewGridAction;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_H
