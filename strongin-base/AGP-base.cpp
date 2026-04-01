#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>

#include "../include/solver.h"

using namespace std;

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	vector<double> dots = { a,b };
	vector<double> value = { func(a, SLOWINGITERS), func(b, SLOWINGITERS) }; // value[i] = значение функции в точке i 
	vector<double> R(1);
	double M;
	double m;

	sort(dots.begin(), dots.end());

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {

		int dotsCount = dots.size();

		for (int i = 0; i < dots.size(); i++) {
			value[i] = func(dots[i], SLOWINGITERS);
		}

		M = fabs((value[dotsCount - 1] - value[dotsCount - 2]) / (dots[dotsCount - 1] - dots[dotsCount - 2]));

		for (int i = 1; i < dots.size(); i++) {
			M = max(M, fabs((value[i] - value[i - 1]) / (dots[i] - dots[i - 1])));
		}



		if (M > 0) {
			m = r * M;
		}
		else {
			m = 1;
		}

		double Rmax = -INFINITY;
		int maxInd = -1;
		for (int i = 1; i < dots.size(); i++) {
			R[i - 1] = m * (dots[i] - dots[i - 1])
				+ (value[i] - value[i - 1]) * (value[i] - value[i - 1]) / (m * (dots[i] - dots[i - 1]))
				- 2 * (value[i] - value[i - 1]);

			if (R[i - 1] > Rmax) {
				Rmax = R[i - 1];
				maxInd = i - 1;
			}
		}
		R.push_back(0);
		double newDot = 0.5 * (dots[maxInd + 1] + dots[maxInd]) - (value[maxInd + 1] - value[maxInd]) * 0.5 / m;
		dots.push_back(newDot);
		value.push_back(0);

		sort(dots.begin(), dots.end());
		double minLength = dots[1] - dots[0];
		for (int i = 2; i < dots.size(); i++) {
			minLength = min(minLength, dots[i] - dots[i - 1]);
		}
		if (minLength <= E) break;

	}

	double extrArg = dots[0], funcMin = func(dots[0], SLOWINGITERS);
	double closestArg1 = dots[0], closestArg2 = dots[1];
	for (int i = 0; i < dots.size(); i++) {
		if (i > 0) {
			if ((dots[i] - dots[i - 1]) < (closestArg2 - closestArg1)) {
				closestArg1 = dots[i - 1];
				closestArg2 = dots[i];
			}
		}
		double dot = dots[i];
		if (func(dot, SLOWINGITERS) < funcMin) {
			funcMin = func(dot, SLOWINGITERS);
			extrArg = dot;
		}
	}
	info res = { extrArg, funcMin, {closestArg1, closestArg2} , dots.size() - 2u};
	return res;
}


int main() {
	Solver solver(AGP, "base");
	solver.init();
	
	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
