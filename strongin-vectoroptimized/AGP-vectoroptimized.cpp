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

struct dotInfo {
	double arg;
	double val;
	dotInfo(double nArg = 0.0, double nVal = 0.0) : arg(nArg), val(nVal) {}
	bool operator<(const dotInfo& rvalue) {
		return arg < rvalue.arg;
	}
};

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	vector<dotInfo> dots;
	dots.push_back({ a, func(a, SLOWINGITERS) });
	dots.push_back({ b, func(b, SLOWINGITERS) });
	double M = fabs((dots[1].val - dots[0].val) / (b - a));
	double m;

	if (M > 0) {
		m = r * M;
	}
	else {
		m = 1;
	}
	double prevm = m;

	vector<double> R(1);
	double firstR = m * (b - a);
	firstR += (dots[1].val - dots[0].val) * (dots[1].val - dots[0].val) / (m * (b - a));
	firstR -= 2 * (dots[1].val - dots[0].val);
	R[0] = firstR;

	double Rmax = firstR;
	int Rmaxindex = 0;

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {

		double rdot = dots[Rmaxindex + 1].arg; // правая граница подразбиваемого интервала
		double ldot = dots[Rmaxindex].arg; // левая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (dots[Rmaxindex + 1].val - dots[Rmaxindex].val) * 0.5 / m;

		dots.push_back({ newDot, func(newDot, SLOWINGITERS) });
		sort(dots.begin(), dots.end());

		if ((dots[Rmaxindex + 1].arg - dots[Rmaxindex].arg) < E || (dots[Rmaxindex + 2].arg - dots[Rmaxindex + 1].arg) < E)
			break;

		double Mcandidate1 = fabs((dots[Rmaxindex + 1].val - dots[Rmaxindex].val) / (dots[Rmaxindex + 1].arg - dots[Rmaxindex].arg));
		double Mcandidate2 = fabs((dots[Rmaxindex + 2].val - dots[Rmaxindex + 1].val) / (dots[Rmaxindex + 2].arg - dots[Rmaxindex + 1].arg));

		M = max({ M, Mcandidate1, Mcandidate2 });

		if (M > 0) {
			m = r * M;
		}
		else {
			m = 1;
		}

		if (prevm != m) { // пересчет R для всех интервалов, т.к. меняется m:
			R.push_back(0);
			Rmax = R[R.size() - 1];
			Rmaxindex = R.size() - 1;

			for (int i = 1; i < dots.size(); i++) {
				R[i - 1] = m * (dots[i].arg - dots[i - 1].arg)
					+ (dots[i].val - dots[i - 1].val) * (dots[i].val - dots[i - 1].val) / (m * (dots[i].arg - dots[i - 1].arg))
					- 2 * (dots[i].val - dots[i - 1].val);
				if (R[i - 1] > Rmax) {
					Rmax = R[i - 1];
					Rmaxindex = i - 1;
				}
			}
		}
		else { // пересчитываем только для нового интервала:
			int i = Rmaxindex + 1;
			R[i - 1] = m * (dots[i].arg - dots[i - 1].arg)
				+ (dots[i].val - dots[i - 1].val) * (dots[i].val - dots[i - 1].val) / (m * (dots[i].arg - dots[i - 1].arg))
				- 2 * (dots[i].val - dots[i - 1].val);
			i++;
			R.push_back(0);
			for (int j = R.size() - 1; j > i - 1; j--) {
				swap(R[j], R[j - 1]);
			}
			R[i - 1] = m * (dots[i].arg - dots[i - 1].arg)
				+ (dots[i].val - dots[i - 1].val) * (dots[i].val - dots[i - 1].val) / (m * (dots[i].arg - dots[i - 1].arg))
				- 2 * (dots[i].val - dots[i - 1].val);

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

	double resArg = dots[0].arg, funcMin = dots[0].val;
	double closestArg1 = dots[0].arg, closestArg2 = dots[1].arg;

	for (int i = 0; i < dots.size(); i++) {
		if (i > 0) {
			if ((dots[i].arg - dots[i - 1].arg) < (closestArg2 - closestArg1)) {
				closestArg1 = dots[i - 1].arg;
				closestArg2 = dots[i].arg;
			}
		}

		double dot = dots[i].arg;
		if (dots[i].val < funcMin) {
			funcMin = dots[i].val;
			resArg = dot;
		}
	}

	info res = { resArg, funcMin, {closestArg1, closestArg2} , dots.size() - 2u };
	return res;
}

int main() {
	Solver solver(AGP, "vectoroptimized");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
