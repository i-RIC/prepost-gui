#include "structured15dgridwithcrosssectionhecrasexporter.h"
#include <guicore/pre/grid/structured15dgridwithcrosssection.h>
#include <guicore/pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercellcontainer.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/project/inputcond/inputconditioncontainerreal.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>
#include <misc/mathsupport.h>

#include <QObject>
#include <QFile>
#include <QTextStream>

namespace {

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

bool Structured15DGridWithCrossSectionHecRasExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, CoordinateSystem* /*cs*/, QWidget* /*parent*/)
{
	auto g = dynamic_cast<Structured15DGridWithCrossSection*>(grid);

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

	double distanceFromFirst = 0;
	auto att = dynamic_cast<GridComplexAttributeContainer*> (g->gridAttribute("Crosssection"));
	for (int i = 0; i < g->crossSections().size(); ++i) {
		Structured15DGridWithCrossSectionCrossSection* cs = g->crossSections().at(i);
		if (i != 0) {
			auto prev = g->vertex(i - 1);
			auto curr = g->vertex(i);
			distanceFromFirst += iRIC::distance(prev, curr);
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
		double lb = cs->altitudeInfo().first().m_position;
		double rb = cs->altitudeInfo().last().m_position;
		o << rb - lb;  // right bank stationing
		o << distanceFromFirst << distanceFromFirst << distanceFromFirst;
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
				for (int i = 1; i < xvals.size(); ++i) {
					pos_vec.push_back(xvals.at(i) - xvals.at(0));
					n_vec.push_back(nvals.at(i));
				}
			} else if (n_type.value() == 2) {
				// horizontal + vertical distribution
				simple = false;
				const InputConditionContainerFunctional& n_h_v = n_h_v_it->second;
				auto xvals = n_h_v.param();
				auto nvals = n_h_v.value("BOTN");
				for (int i = 1; i < xvals.size(); ++i) {
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
			o.setFieldWidth(0);
			o << " ";
			o.setFieldWidth(7);
			o << n;
			o.setFieldWidth(0);
			o << " ";
			o.setFieldWidth(7);
			o << rb - lb;
			o.setFieldWidth(0);
			o << endl;
		} else {
			int idx = 0;
			while (idx < pos_vec.size()) {
				if (idx == 0) {
					o.setFieldWidth(0);
					o << "NH ";
					o.setFieldWidth(5);
					o << n_vec.size();
					o.setFieldWidth(0);
					o << " ";
					o.setFieldWidth(7);
					o << n_vec.at(0);
					o.setFieldWidth(0);
					o << " ";
					o.setFieldWidth(7);
					o << pos_vec.at(0);
				} else if ((idx + 1) % 5 == 0) {
					o.setFieldWidth(0);
					o << " ";
					o.setFieldWidth(7);
					o << n_vec.at(idx);
					o.setFieldWidth(0);
					o << endl;
					o << "NH ";
					if (pos_vec.at(idx) >= 1000) {
						o.setRealNumberPrecision(0);
					} else if (pos_vec.at(idx) >= 100) {
						o.setRealNumberPrecision(1);
					} else {
						o.setRealNumberPrecision(2);
					}
					o.setFieldWidth(5);
					o << pos_vec.at(idx);
					o.setRealNumberPrecision(2);
				}
				else {
					o.setFieldWidth(0);
					o << " ";
					o.setFieldWidth(7);
					o << n_vec.at(idx);
					o.setFieldWidth(0);
					o << " ";
					o.setFieldWidth(7);
					o << pos_vec.at(idx);
				}
				++ idx;
			}
			o << endl;
		}

		// GR values
		int row_idx = 0;
		o.setRealNumberPrecision(1);
		for (const Structured15DGridWithCrossSectionCrossSection::Altitude& alt : cs->altitudeInfo()) {
			if (row_idx == 0) {
				o.setFieldWidth(0);
				o << "GR ";
				o.setFieldWidth(5);
				o << alt.m_height;
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.m_position;
			} else {
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.m_height;
				o.setFieldWidth(0);
				o << " ";
				o.setFieldWidth(7);
				o << alt.m_position;
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
