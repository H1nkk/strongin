#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <numbers>
#include <set>
#include <chrono>
#include <vector>
#include <filesystem>
#include "pqueue.h"

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
	RInfo(double nR = 0, double nArg = 0) : R(nR), arg(nArg) {}
	bool operator<(const RInfo& rvalue) const {
		return R < rvalue.R;
	}
	bool operator<=(const RInfo& rvalue) const {
		return R <= rvalue.R;
	}
	bool operator>(const RInfo& rvalue) const {
		return R > rvalue.R;
	}
	bool operator>=(const RInfo& rvalue) const {
		return R >= rvalue.R;
	}
	bool operator==(const RInfo& rvalue) const {
		return (R == rvalue.R);
	}
};

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	// инициализация
	size_t pqueueLimit = ITERMAX / 2;
	map<double, double> funcValue; // мапа из аргумента в значение функции 
	double firstM = fabs((func(b, SLOWINGITERS) - func(a, SLOWINGITERS)) / (b - a));
	funcValue[a] = func(a, SLOWINGITERS);
	funcValue[b] = func(b, SLOWINGITERS);

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

	pqueue_limited_linear<RInfo> Rqueue(pqueueLimit);
	pqueue<RInfo> Rqueue_unlim;

	double firstR = m * (b - a);
	firstR += (rightFuncVal - leftFuncVal) * (rightFuncVal - leftFuncVal) / (m * (b - a));
	firstR -= 2 * (rightFuncVal - leftFuncVal);
	Rqueue.insert({ firstR, a });
	Rqueue_unlim.insert({ firstR, a });

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Добавление новой точки
		RInfo maxInfo = Rqueue.get();
		RInfo maxInfo_unlim = Rqueue_unlim.get();

		if (maxInfo != maxInfo_unlim) {
			cout << "unasdf";
		}
		double ldot = maxInfo.arg; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;
		funcValue[newDot] = func(newDot, SLOWINGITERS);

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
			Rqueue_unlim.clear();
			for (int i = 0; i < funcValue.size() - 1; i++) {
				double ldot = (*prev).first, rdot = (*cur).first;
				double lval = funcValue[ldot], rval = funcValue[rdot];
				double newR = m * (rdot - ldot)
					+ (rval - lval) * (rval - lval) / (m * (rdot - ldot))
					- 2 * (rval - lval);
				Rqueue.insert({ newR, ldot });
				Rqueue_unlim.insert({ newR, ldot });
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
				- 2 * (funcValue[rArg] - funcValue[lArg]); // = Rmax

			Rqueue.pop();
			Rqueue_unlim.pop();

			double newR1 = m * (mArg - lArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
				- 2 * (funcValue[mArg] - funcValue[lArg]);
			double newR2 = m * (rArg - mArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
				- 2 * (funcValue[rArg] - funcValue[mArg]);

			Rqueue.insert({ newR1, lArg });
			Rqueue.insert({ newR2, mArg });

			Rqueue_unlim.insert({ newR1, lArg });
			Rqueue_unlim.insert({ newR2, mArg });
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
	info res = { extrArg, funcMin, {closestArg1, closestArg2} ,funcValue.size() - 2 };
	return res;
}

int main() {
	Solver solver(AGP, "pqlimitedsz");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
