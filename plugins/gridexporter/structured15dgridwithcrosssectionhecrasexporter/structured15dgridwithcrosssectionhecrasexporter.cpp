#include "structured15dgridwithcrosssectionhecrasexporter.h"

#include <guicore/grid/v4structured15dgridwithcrosssection.h>
#include <guicore/grid/v4structured15dgridwithcrosssectioncrosssection.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/project/inputcond/inputconditioncontainerreal.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>
#include <misc/mathsupport.h>

#include <QObject>
#include <QFile>
#include <QTextStream>

namespace {

void outputSingleSpace(QTextStream *s)
{
	s->setFieldWidth(0);
	*s << " ";
}

void outputRealValue(QTextStream* s, double value, int fieldWidth, int precision)
{
	s->setRealNumberPrecision(precision);
	s->setFieldWidth(fieldWidth);
	*s << value;
}

void outputPosValue(QTextStream* s, double value, int fieldWidth)
{
	int precision = 2;

	if (fieldWidth == 5) {
		if (value >= 1000) {
			precision = 0;
		} else if (value  >= 100) {
			precision = 1;
		}
	}
	outputRealValue(s, value, fieldWidth, precision);
}

void outputNValue(QTextStream* s, double value)
{
	outputRealValue(s, value, 7, 3);
}

} // namespace

Structured15DGridWithCrossSectionHecRasExporter::Structured15DGridWithCrossSectionHecRasExporter() :
	QObject()
{}

Structured15DGridWithCrossSectionHecRasExporter::~Structured15DGridWithCrossSectionHecRasExporter()
{}

QString Structured15DGridWithCrossSectionHecRasExporter::caption() const
{
	return tr("HEC-RAS");
}

bool Structured15DGridWithCrossSectionHecRasExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return gt == SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
}

QStringList Structured15DGridWithCrossSectionHecRasExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("HEC-RAS text files (*.dat)");
	return ret;
}

bool Structured15DGridWithCrossSectionHecRasExporter::doExport(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	auto g = dynamic_cast<v4Structured15dGridWithCrossSection*> (grid->grid());

	QFile f(filename);
	if (! f.open(QFile::WriteOnly | QIODevice::Text)){return false;}

	QTextStream o(&f);
	o.setRealNumberNotation(QTextStream::FixedNotation);

	o << "T1      HEC-RAS" << endl;
	o << "T2      " << endl;
	o << "T3      " << endl;
	o << "*" << endl;
	o << "J1                                             1" << endl;
	o << "*" << endl;
	o << "QT" << endl;
	o << "*" << endl;

	double distance = 0;
	auto att = dynamic_cast<GridComplexAttributeContainer*> (grid->attribute("Crosssection"));
	const auto gsize = g->crossSections().size();
	for (int i = 0; i < static_cast<int> (g->crossSections().size()); ++i) {
		auto cs = g->crossSections().at(gsize - 1 - i);
		if (i != 0) {
			auto prev = g->point2d(gsize - i);
			auto curr = g->point2d(gsize - 1 - i);
			distance = iRIC::distance(prev, curr);
		}
		// X1 record
		o.setFieldWidth(0);
		o << "X1 ";
		o.setRealNumberPrecision(2);
		o.setFieldWidth(5);
		o << g->crossSections().size() - i; // section number
		o.setFieldWidth(8);
		o << cs->altitudeInfo().size(); // number of points in cross section
		o << 0; // left bank stationing (always 0)
		double lb = cs->altitudeInfo().begin()->position;
		double rb = cs->altitudeInfo().rbegin()->position;
		o << rb - lb;  // right bank stationing
		o << distance << distance << distance;
		o.setFieldWidth(0);
		o << endl;

		// NH value
		GridComplexConditionGroup* group = att->groups().at(i);

		const auto& ints = group->containerSet()->integers();
		const auto& reals = group->containerSet()->reals();
		const auto& funcs = group->containerSet()->functionals();

		auto n_type_it = ints.find("n_type");
		auto n_simple_it = reals.find("n_simple");
		auto n_h_it = funcs.find("n_h");
		auto n_h_v_it = funcs.find("n_h_v");

		bool simple = true;
		double n = 0.03;
		std::vector<double> pos_vec;
		std::vector<double> n_vec;

		if (n_type_it == ints.end() || n_simple_it == reals.end() || n_h_it == funcs.end() || n_h_v_it == funcs.end()) {
			// not SAC
		} else {
			const InputConditionContainerInteger& n_type = n_type_it->second;
			if (n_type.value() == 0) {
				// simple
				const InputConditionContainerReal& n_simple = n_simple_it->second;
				n = n_simple.value();
			} else if (n_type.value() == 1) {
				// horizontal distribution
				simple = false;
				const InputConditionContainerFunctional& n_h = n_h_it->second;
				auto xvals = n_h.param();
				auto nvals = n_h.value("NVAL");
				for (int i = 1; i < static_cast<int> (xvals.size()); ++i) {
					pos_vec.push_back(xvals.at(i) - xvals.at(0));
					n_vec.push_back(nvals.at(i));
				}
			} else if (n_type.value() == 2) {
				// horizontal + vertical distribution
				simple = false;
				const InputConditionContainerFunctional& n_h_v = n_h_v_it->second;
				auto xvals = n_h_v.param();
				auto nvals = n_h_v.value("BOTN");
				for (int i = 1; i < static_cast<int> (xvals.size()); ++i) {
					pos_vec.push_back(xvals.at(i) - xvals.at(0));
					n_vec.push_back(nvals.at(i));
				}
			}
		}
		if (simple) {
			o.setFieldWidth(0);
			o << "NH ";
			o.setFieldWidth(5);
			o << 1;

			outputSingleSpace(&o);
			outputNValue(&o, n);

			outputSingleSpace(&o);
			outputPosValue(&o, rb - lb, 7);

			o.setFieldWidth(0);
			o << endl;
		} else {
			std::vector<double> pos_vec2, n_vec2;
			int idx = 0;
			while (idx < static_cast<int> (pos_vec.size())) {
				if (idx < n_vec.size() - 1 && n_vec.at(idx) == n_vec.at(idx + 1)) {
					++ idx;
					continue;
				}
				pos_vec2.push_back(pos_vec.at(idx));
				n_vec2.push_back(n_vec.at(idx));
				++ idx;
			}

			idx = 0;
			while (idx < pos_vec2.size()) {
				if (idx == 0) {
					o.setFieldWidth(0);
					o << "NH ";

					o.setFieldWidth(5);
					o << n_vec2.size();

					outputSingleSpace(&o);
					outputNValue(&o, n_vec2.at(idx));

					outputSingleSpace(&o);
					outputPosValue(&o, pos_vec2.at(idx), 7);
				} else if ((idx + 1) % 5 == 0) {
					outputSingleSpace(&o);
					outputNValue(&o, n_vec2.at(idx));

					o.setFieldWidth(0);
					o << endl;
					o << "NH ";

					outputPosValue(&o, pos_vec2.at(idx), 5);
				} else {
					outputSingleSpace(&o);
					outputNValue(&o, n_vec2.at(idx));

					outputSingleSpace(&o);
					outputPosValue(&o, pos_vec2.at(idx), 7);
				}
				++ idx;
			}
			o << endl;
		}

		// GR values
		int row_idx = 0;
		o.setRealNumberPrecision(1);
		for (const auto& alt : cs->altitudeInfo()) {
			if (row_idx == 0) {
				o.setFieldWidth(0);
				o << "GR";
				o.setFieldWidth(6);
				o << alt.height;
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.position;
			} else {
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.height;
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.position;
			}
			++ row_idx;

			if (row_idx == 5) {
				row_idx = 0;
				o.setFieldWidth(0);
				o << endl;
			}
		}
		if (row_idx != 0) {
			o.setFieldWidth(0);
			o << endl;
		}
	}

	f.close();
	return true;
}
