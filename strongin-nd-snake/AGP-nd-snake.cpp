#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>

#include "../strongin-pqueue/pqueue.h"
#include "../include/solver.h"

using namespace std;

struct dotInfo {
	double arg;
	double funcVal;
	double R;
};

struct RInfo {
	double R;
	double arg;
	//pair<double, double>* info;
	RInfo(double nR = 0, double nArg = 0) : R(nR), arg(nArg) {}
	//RInfo(double nR, pair<double, double>* di) : R(nR), info(di) {}
	bool operator<(const RInfo& rvalue) {
		return R < rvalue.R;
	}
	bool operator<=(const RInfo& rvalue) {
		return R <= rvalue.R;
	}
	bool operator>(const RInfo& rvalue) {
		return R > rvalue.R;
	}
	bool operator>=(const RInfo& rvalue) {
		return R >= rvalue.R;
	}
	bool operator==(const RInfo& rvalue) {
		return (R == rvalue.R);
	}
};

pair<double, double> get_cell_center(long long i, long long n) {
	long long row = i / n;
	long long col;
	if (row % 2 == 0) {
		col = i % n;
	}
	else {
		col = n - 1 - i % n;
	}

	return { (col + 0.5) / n, (row + 0.5) / n };
}

pair<double, double> snake(double x, long long n) {
	double t = x * (n * n - 1);
	long long i = t;
	if (i >= n * n - 1) i = n * n - 2;
	double delta = t - i;

	auto center_i = get_cell_center(i, n);
	auto center_i_plus = get_cell_center(i + 1, n);
	pair<double, double> res = make_pair(
		center_i.first + delta * (center_i_plus.first - center_i.first),
		center_i.second + delta * (center_i_plus.second - center_i.second)
	);

	return res;
}

double f(double x, double y) {
	return (x - 6) * (x - 6) + (y - 4) * (y - 4);
}

double f(pair<double, double> dot) {
	double x = dot.first;
	double y = dot.second;
	return (x - 6) * (x - 6) + (y - 4) * (y - 4);
}

pair<double, double> to_box(double x, long long n,
	double x_min, double x_max, double y_min, double y_max) {
	auto p = snake(x, n);
	return { x_min + p.first * (x_max - x_min),
			 y_min + p.second * (y_max - y_min) };
}

info AGP(double x_min, double x_max, double y_min, double y_max,
	double (*func)(pair<double, double>), double r, double E, int ITERMAX, int SLOWINGITERS, long long n) {
	// инициализация
	const double a = 0, b = 1;
	const double x_len = x_max - x_min;
	const double y_len = y_max - y_min;
	E /= n;

	map<double, double> funcValue; // мапа из аргумента в значение функции 

	pair<double, double> left_dot = to_box(a, n, x_min, x_max, y_min, y_max);
	pair<double, double> right_dot = to_box(b, n, x_min, x_max, y_min, y_max);


	double firstM = fabs((func(right_dot) - func(left_dot)) / (b - a));
	funcValue[a] = func(left_dot);
	funcValue[b] = func(right_dot);

	double rightFuncVal = funcValue[b], leftFuncVal = funcValue[a];
	double M = fabs((rightFuncVal - leftFuncVal) / (b - a));
	double m;
	double prevm = 0;

	if (M > 0) {
		m = r * M;
	}
	else {
		m = 1;
	}

	pqueue<RInfo> Rqueue;

	double firstR = m * (b - a);
	firstR += (rightFuncVal - leftFuncVal) * (rightFuncVal - leftFuncVal) / (m * (b - a));
	firstR -= 2 * (rightFuncVal + leftFuncVal);
	Rqueue.insert({ firstR, a });

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Добавление новой точки
		RInfo maxInfo = Rqueue.get();
		double ldot = maxInfo.arg; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;
		pair<double, double> newNDimDot = to_box(newDot, n, x_min, x_max, y_min, y_max);
		funcValue[newDot] = func(newNDimDot);

		if ((rdot - newDot) < E || (newDot - ldot) < E)
			break;

		// Пересчет M для нового интервала
		ldot = maxInfo.arg;
		double mdot = (*next(funcValue.find(ldot))).first;
		rdot = (*next(funcValue.find(mdot))).first;
		double Mcandidate1 = fabs((funcValue[mdot] - (funcValue[ldot])) / (mdot - ldot));
		double Mcandidate2 = fabs((funcValue[rdot] - (funcValue[mdot])) / (rdot - mdot));
		M = max({ M,Mcandidate1,Mcandidate2 });

		if (M > 0) {
			m = r * M;
		}
		else {
			m = 1;
		}

		if (prevm != m) {
			auto prev = funcValue.begin();
			auto cur = next(funcValue.begin());

			Rqueue.clear();
			for (int i = 0; i < funcValue.size() - 1; i++) {
				double ldot = (*prev).first, rdot = (*cur).first;
				double lval = funcValue[ldot], rval = funcValue[rdot];
				double newR = m * (rdot - ldot)
					+ (rval - lval) * (rval - lval) / (m * (rdot - ldot))
					- 2 * (rval + lval);
				Rqueue.insert({ newR, ldot });
				prev = next(prev);
				cur = next(cur);
			}

		}
		else {

			double lArg = Rqueue.get().arg; // аргумент, для которого будем пересчитывать R
			double mArg = newDot; // этот аргумент только что появился, для него нужно посчитать R
			double rArg = (*next(funcValue.find(newDot))).first; // правая граница нового интервала

			double RToRecalculate1 = m * (rArg - lArg)
				+ (funcValue[rArg] - funcValue[lArg]) * (funcValue[rArg] - funcValue[lArg]) / (m * (rArg - lArg))
				- 2 * (funcValue[rArg] + funcValue[lArg]);

			Rqueue.pop();

			double newR1 = m * (mArg - lArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
				- 2 * (funcValue[mArg] + funcValue[lArg]);
			double newR2 = m * (rArg - mArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
				- 2 * (funcValue[rArg] + funcValue[mArg]);

			Rqueue.insert({ newR1, lArg });
			Rqueue.insert({ newR2, mArg });
		}

		prevm = m;
	}

	double extrArg = (*funcValue.begin()).first;
	double funcMin = funcValue[extrArg];
	double prevArg = -INFINITY;
	double closestArg1 = -INFINITY, closestArg2 = INFINITY;
	for (auto p : funcValue) {
		if (prevArg != -INFINITY) {
			if ((p.first - prevArg) < (closestArg2 - closestArg1)) {
				closestArg1 = prevArg;
				closestArg2 = p.first;
			}
		}
		if (p.second < funcMin) {
			funcMin = p.second;
			extrArg = p.first;
		}
		prevArg = p.first;
	}
	info res = { extrArg, funcMin,{ closestArg1, closestArg2 } ,funcValue.size() - 2 };
	return res;
}



int main() {
	double E = 0.01;
	double x_min = -8, x_max = 8;
	double y_min = -30, y_max = 30;
	double x_len = x_max - x_min;
	double y_len = y_max - y_min;
	long long n = (long long)ceil(max(x_len, y_len) / (2.0 * E)) + 1;
	auto res = AGP(
		x_min, x_max,
		y_min, y_max,
		f,
		2.0,
		E,
		1000,
		0,
		n
	);
	pair<double, double> resDot = snake(res.extremumArg, n);
	resDot.first = resDot.first * x_len + x_min;
	resDot.second = resDot.second * y_len + y_min;

	std::cout << std::fixed;
	cout << resDot.first << " " << resDot.second;
	return 0;
}