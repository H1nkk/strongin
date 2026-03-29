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
#include "dllist.h"

#include "../include/solver.h"


using namespace std;

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	dllist dots({ a,b });

	dllist value({ func(a, SLOWINGITERS), func(b, SLOWINGITERS) }); // value[i] = значение функции в точке i 

	double M = fabs(value[1] - value[0]) / (dots[1] - dots[0]);
	double m;

	if (M > 0) {
		m = r * M;
	}
	else {
		m = 1;
	}
	double firstR = m * (dots[1] - dots[0]);
	firstR += (value[1] - value[0]) * (value[1] - value[0]) / (m * (dots[1] - dots[0]));
	firstR -= 2 * (value[1] - value[0]);

	dllist R;
	R.insert(0, firstR);
	double Rmax = R[0];
	int Rmaxindex = 0;
	double prevm = m;

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {

		double rdot = dots[Rmaxindex + 1]; // правая граница подразбиваемого интервала
		double ldot = dots[Rmaxindex]; // левая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (value[Rmaxindex + 1] - value[Rmaxindex]) * 0.5 / m;
		dots.insert(Rmaxindex + 1, newDot);
		value.insert(Rmaxindex + 1, func(newDot, SLOWINGITERS));

		if ((dots[Rmaxindex + 1] - dots[Rmaxindex]) < E || (dots[Rmaxindex + 2] - dots[Rmaxindex + 1]) < E)
			break;


		// пересчет M для нового интервала:
		double Mcandidate1 = fabs((value[Rmaxindex + 1] - value[Rmaxindex]) / (dots[Rmaxindex + 1] - dots[Rmaxindex]));
		double Mcandidate2 = fabs((value[Rmaxindex + 2] - value[Rmaxindex + 1]) / (dots[Rmaxindex + 2] - dots[Rmaxindex + 1]));

		// поиск наибольшего M:
		M = max({ M, Mcandidate1, Mcandidate2 });

		if (M > 0) {
			m = r * M;
		}
		else {
			m = 1;
		}

		if (prevm != m) { // пересчет R для всех интервалов, т.к. меняется m:
			R.insert(0, 0);
			Rmax = R[0];
			Rmaxindex = 0;

			for (int i = 1; i < dots.size(); i++) {
				R[i - 1] = m * (dots[i] - dots[i - 1])
					+ (value[i] - value[i - 1]) * (value[i] - value[i - 1]) / (m * (dots[i] - dots[i - 1]))
					- 2 * (value[i] - value[i - 1]);
				if (R[i - 1] > Rmax) {
					Rmax = R[i - 1];
					Rmaxindex = i - 1;
				}
			}
		}
		else { // пересчитываем только для нового интервала:
			int i = Rmaxindex + 1;
			R[i - 1] = m * (dots[i] - dots[i - 1])
				+ (value[i] - value[i - 1]) * (value[i] - value[i - 1]) / (m * (dots[i] - dots[i - 1]))
				- 2 * (value[i] - value[i - 1]);
			i++;
			R.insert(i - 1, m * (dots[i] - dots[i - 1])
				+ (value[i] - value[i - 1]) * (value[i] - value[i - 1]) / (m * (dots[i] - dots[i - 1]))
				- 2 * (value[i] - value[i - 1]));

			Rmax = R[0];
			Rmaxindex = 0;

			for (int i = 0; i < R.size(); i++) {
				if (R[i] > Rmax) {
					Rmax = R[i];
					Rmaxindex = i;
				}
			}
		}
		prevm = m;
	}

	double resArg = dots[0], funcMin = value[0];
	double closestArg1 = dots[0], closestArg2 = dots[1];

	for (int i = 0; i < dots.size(); i++) {
		double dot = dots[i];
		if (i > 0) {
			if ((dots[i] - dots[i - 1]) < (closestArg2 - closestArg1)) {
				closestArg1 = dots[i - 1];
				closestArg2 = dots[i];
			}
		}
		if (value[i] < funcMin) {
			funcMin = value[i];
			resArg = dot;
		}
	}

	info res = { resArg, funcMin, {closestArg1, closestArg2} , dots.size() - 2u };
	return res;
}

int main() {
	Solver solver(AGP, "dllist");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
