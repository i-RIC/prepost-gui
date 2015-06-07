#ifndef RAWDATABACKGROUND_H
#define RAWDATABACKGROUND_H

#include "../rawdata/rawdata.h"
#include <QAction>
#include <QVariant>

class RawDataBackgroundEditValueCommand;

class RawDataBackground : public RawData
{
	Q_OBJECT

public:
	enum Type {
		Maximum,
		Minimum,
		Custom
	};
	/// Constructor
	RawDataBackground(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridAttribute* condition);
	void addCustomMenuItems(QMenu* menu) override;
	void handleStandardItemDoubleClicked() override {editValue();}
	void setupDataItem() override;
	const QVariant& customValue() const {return m_customValue;}
	virtual QVariant variantValue();
	Type type() const {return m_type;}
	void setType(Type t) {m_type = t;}
	bool getValueRange(double* min, double* max) override;
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

private slots:
	void editValue();

protected:
	QVariant m_customValue;
	Type m_type;
private:
	QAction* m_editValueAction;
public:
	friend class RawDataBackgroundEditValueCommand;
};

#endif // RAWDATABACKGROUND_H
