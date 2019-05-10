#ifndef DVECTOR_H
#define DVECTOR_H

#include <vector>
#include <cmath>

using my_vector = std::vector<double>;

inline my_vector& operator+=(my_vector& lhs, const my_vector& rhs) {
	for (std::size_t i = 0; i < lhs.size(); i++)
		lhs[i] += rhs[i];
	return lhs;
}

inline my_vector& operator-=(my_vector& lhs, const my_vector& rhs) {
	for (std::size_t i = 0; i < lhs.size(); i++)
		lhs[i] -= rhs[i];
	return lhs;
}

inline my_vector& operator*=(my_vector& lhs, const my_vector& rhs) {
	for (std::size_t i = 0; i < lhs.size(); i++)
		lhs[i] *= rhs[i];
	return lhs;
}

inline my_vector& operator/=(my_vector& lhs, const my_vector& rhs) {
	for (std::size_t i = 0; i < lhs.size(); i++)
		lhs[i] /= rhs[i];
	return lhs;
}

inline my_vector& operator+=(my_vector& lhs, const double rhs) {
	for (auto& e : lhs) e += rhs;
	return lhs;
}

inline my_vector& operator-=(my_vector& lhs, const double rhs) {
	for (auto& e : lhs) e -= rhs;
	return lhs;
}

inline my_vector& operator*=(my_vector& lhs, const double rhs) {
	for (auto& e : lhs) e *= rhs;
	return lhs;
}

inline my_vector& operator/=(my_vector& lhs, const double rhs) {
	for (auto& e : lhs) e /= rhs;
	return lhs;
}

inline my_vector operator+(const my_vector& lhs, const my_vector& rhs) {
	return my_vector(lhs) += rhs;
}

inline my_vector operator-(const my_vector& lhs, const my_vector& rhs) {
	return my_vector(lhs) -= rhs;
}

inline my_vector operator*(const my_vector& lhs, const my_vector& rhs) {
	return my_vector(lhs) *= rhs;
}

inline my_vector operator/(const my_vector& lhs, const my_vector& rhs) {
	return my_vector(lhs) /= rhs;
}

inline my_vector operator+(const my_vector& lhs, const double rhs) {
	return my_vector(lhs) += rhs;
}

inline my_vector operator-(const my_vector& lhs, const double rhs) {
	return my_vector(lhs) -= rhs;
}

inline my_vector operator*(const my_vector& lhs, const double rhs) {
	return my_vector(lhs) *= rhs;
}

inline my_vector operator/(const my_vector& lhs, const double rhs) {
	return my_vector(lhs) /= rhs;
}

inline double mean(const my_vector& obj) {
	const std::size_t size = obj.size();
	if (size == 0) return 0;
	double t = 0;
	for (auto e : obj) t += e;
	return t / size;
}

inline double sd(const my_vector& obj) {
	const std::size_t size = obj.size();
	if (size < 2) return 0;
	double m = mean(obj);
	double t = 0;
	for (auto e : obj) t += (e - m) * (e - m);
	return std::sqrt(t / (size - 1));
}

inline double norm(const my_vector& obj) {
	double t = 0;
	for (auto e : obj) t += e * e;
	return std::sqrt(t);
}

#endif // DVECTOR_H
