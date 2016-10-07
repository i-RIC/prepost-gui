#ifndef IRICLIB_CGNSFILE_BASEITERATIVET_H
#define IRICLIB_CGNSFILE_BASEITERATIVET_H

#include <string>
#include <vector>

template <typename V>
class BaseIterativeT
{
public:
	BaseIterativeT(const std::string& name);

	std::string name() const;

	const std::vector<V>& values() const;
	void setValues(const std::vector<V>& vals);
	void addValue(V newVal);

private:
	std::string m_name;
	std::vector<V> m_values;
};

#include "iriclib_cgnsfile_baseiterativet_detail.h"

#endif // IRICLIB_CGNSFILE_BASEITERATIVET_H
