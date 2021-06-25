#ifndef GEODATABACKGROUND_H
#define GEODATABACKGROUND_H

#include "../geodata/geodata.h"
#include <QVariant>

class QAction;

class GeoDataBackground : public GeoData
{
	Q_OBJECT

public:
	enum Type {
		Maximum,
		Minimum,
		Custom
	};

	GeoDataBackground(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	void addCustomMenuItems(QMenu* menu) override;
	void handleStandardItemDoubleClicked() override {editValue();}
	void setupDataItem() override;
	const QVariant& customValue() const;
	virtual QVariant variantValue();

	Type type() const;
	void setType(Type t);

	bool getValueRange(double* min, double* max) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

private slots:
	void editValue();

private:
	QVariant m_customValue;
	Type m_type;

	QAction* m_editValueAction;

	class EditValueCommand;
};

#endif // GEODATABACKGROUND_H
