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

#include "../include/solver.h"

using namespace std;

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	// инициализация
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

	multimap<double, double> RtoArg; // в RtoArg[curR] хранится аргумент x, с которого начинается отрезок для характеристикой R = curR
	double firstR = m * (b - a);
	firstR += (rightFuncVal - leftFuncVal) * (rightFuncVal - leftFuncVal) / (m * (b - a));
	firstR -= 2 * (rightFuncVal - leftFuncVal);
	RtoArg.insert(make_pair(firstR, a));

	double Rmax = firstR;
	int Rmaxindex = 0;

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Добавление новой точки
		double ldot = RtoArg.find(Rmax)->second; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;
		funcValue[newDot] = func(newDot, SLOWINGITERS);

		if ((rdot - newDot) < E || (newDot - ldot) < E)
			break;

		// Пересчет M для нового интервала
		double mdot = (*next(funcValue.find(ldot))).first;
		//rdot = (*next(funcValue.find(mdot))).first;
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

			RtoArg.clear();
			for (int i = 0; i < funcValue.size() - 1; i++) {
				double ldot = (*prev).first, rdot = (*cur).first;
				double lval = funcValue[ldot], rval = funcValue[rdot];
				double newR = m * (rdot - ldot)
					+ (rval - lval) * (rval - lval) / (m * (rdot - ldot))
					- 2 * (rval - lval);
				//RtoArg[newR] = ldot;
				RtoArg.insert(make_pair(newR, ldot));
				prev = next(prev);
				cur = next(cur);
			}

		}
		else {

			Rmax = (*prev(RtoArg.end())).first;
			double lArg = RtoArg.find(Rmax)->second; // аргумент, для которого будем пересчитывать R
			double mArg = newDot; // этот аргумент только что появился, для него нужно посчитать R
			double rArg = (*next(funcValue.find(newDot))).first; // правая граница нового интервала

			double RToRecalculate1 = m * (rArg - lArg)
				+ (funcValue[rArg] - funcValue[lArg]) * (funcValue[rArg] - funcValue[lArg]) / (m * (rArg - lArg))
				- 2 * (funcValue[rArg] - funcValue[lArg]);

			RtoArg.erase(RtoArg.find(RToRecalculate1));

			double newR1 = m * (mArg - lArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
				- 2 * (funcValue[mArg] - funcValue[lArg]);
			double newR2 = m * (rArg - mArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
				- 2 * (funcValue[rArg] - funcValue[mArg]);

			//RtoArg[newR1] = lArg;
			RtoArg.insert(make_pair(newR1, lArg));
			RtoArg.insert(make_pair(newR2, mArg));
		}

		prevm = m;
		Rmax = (*prev(RtoArg.end())).first;
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
	info res = { extrArg, funcMin, {closestArg1, closestArg2} ,funcValue.size() - 2};
	return res;
}



int main() {
	Solver solver(AGP, "map");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
