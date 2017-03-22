#include "crosssections.h"
#include "crosssectionspreprocessorcontroller.h"
#include "../base/dataitemview.h"
#include "../crosssection/crosssection.h"

#include "private/crosssections_impl.h"

#include <QFile>
#include <QStandardItem>
#include <QTextStream>
#include <QVector3D>

CrossSections::CrossSections(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

CrossSections::~CrossSections()
{
	delete impl;
}

CrossSection* CrossSections::addCrossSection()
{
	auto newS = new CrossSection(this);
	newS->setId(static_cast<int>(childItems().size() - 1));

	return newS;
}

std::vector<CrossSection*> CrossSections::crossSectionVector() const
{
	std::vector<CrossSection*> ret;
	for (auto c : childItems()) {
		ret.push_back(dynamic_cast<CrossSection*> (c));
	}
	return ret;
}

bool CrossSections::exportRiverSurveyData(const QString& fileName) const
{
	QFile file(fileName);
	if (! file.open(QIODevice::WriteOnly)) {return false;}

	QTextStream ts(&file);

	ts << "#survey" << endl;
	for (CrossSection* cs : crossSectionVector()) {
		ts
				<< (cs->id() + 1) << " "
				<< cs->point1().x() << " "
				<< cs->point1().y() << " "
				<< cs->point2().x() << " "
				<< cs->point2().y() << endl;
	}
	ts << endl;

	ts << "#x-section" << endl;
	for (CrossSection* cs : crossSectionVector()) {
		auto points = cs->mappedPoints();
		ts << (cs->id() + 1) << " " << points.size() << endl;
		QPointF mappedPoint;
		double position;
		std::map<double, double> elevs;
		for (QVector3D* p : points) {
			cs->getMappedPoint(p->x(), p->y(), &mappedPoint, &position);
			elevs.insert(std::make_pair(position, p->z()));
		}
		int index = 0;
		for (auto& pair : elevs) {
			ts << " " << pair.first << " " << pair.second;
			if (index % 4 == 3) {ts << endl;}
			++ index;
		}
		ts << endl;
	}
}

QStandardItem* CrossSections::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconFolder.png"), tr("Cross Sections"));
	setupStandardItem(item);
	return item;
}

DataItemController* CrossSections::buildPreProcessorDataItemController(Model* model)
{
	return new CrossSectionsPreProcessorController(model, this);
}

DataItemView* CrossSections::buildPreProcessorDataItemView(Model* model)
{
	return new DataItemView(model, this);
}
