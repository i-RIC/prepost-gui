#ifndef HYDRAULICDATA_H
#define HYDRAULICDATA_H

#include "../../guicore_global.h"
#include "../../project/projectdataitem.h"

class HydraulicDataCreator;
class PreProcessorHydraulicDataDataItemInterface;

class QUndoCommand;

class GUICOREDLL_EXPORT HydraulicData : public ProjectDataItem
{
public:
	HydraulicData(ProjectDataItem* parent);
	virtual ~HydraulicData();

	QString name() const;
	void setName(const QString& name);

	QString caption() const;
	void setCaption(const QString& caption);

	virtual bool dataExists() const = 0;

protected:
  PreProcessorHydraulicDataDataItemInterface* hydraulicDataDataItem() const;
  void pushCommand(QUndoCommand* com);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/hydraulicdata_impl.h"
#endif // _DEBUG

#endif // HYDRAULICDATA_H
