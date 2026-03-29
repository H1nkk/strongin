#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <map>
#include <numbers>
#include <set>
#include <chrono>
#include <vector>
#include <filesystem>

#include "../include/solver.h"

using namespace std;

info custom_AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	// инициализация
	map<double, double> funcValue; // мапа из аргумента в значение функции 
	double firstM = fabs((func(b, SLOWINGITERS) - func(a, SLOWINGITERS)) / (b - a));
	funcValue[a] = func(a, SLOWINGITERS);
	funcValue[b] = func(b, SLOWINGITERS);

	multimap<double, double> RtoArg; // в RtoArg[curR] хранится аргумент x, с которого начинается отрезок для характеристикой R = curR
	double firstR = (b - a);
	RtoArg.insert(make_pair(firstR, a));

	int threads_count = 1;

	int iterations_done = 0;
	while (iterations_done < ITERMAX) {

		double Rmax = prev(RtoArg.end())->first;
		double ldot = RtoArg.find(Rmax)->second; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot);

		funcValue[newDot] = 0;

		double RToRecalculate1 = rdot - ldot;

		RtoArg.erase(RtoArg.find(RToRecalculate1));

		double newR1 = (newDot - ldot);
		double newR2 = (rdot - newDot);

		RtoArg.insert(make_pair(newR1, ldot));
		RtoArg.insert(make_pair(newR2, newDot));


		iterations_done++;

		if ((rdot - newDot) < E || (newDot - ldot) < E) {
			break;
		}
	}

	vector<double> args;
	for (auto [arg, val] : funcValue) {
		args.push_back(arg);
	}
	vector<double> func_values_vec(args.size());
	assert(args.size() == iterations_done + 2);


	for (int i = 0; i < iterations_done + 2; i++) {
		func_values_vec[i] = func(args[i], SLOWINGITERS);
	}

	for (int i = 0; i < iterations_done + 2; i++) {
		funcValue[args[i]] = func_values_vec[i];
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
	info res = { extrArg, funcMin, {closestArg1, closestArg2} , (size_t)iterations_done };
	return res;
}

int main() {
	Solver solver(custom_AGP, "simple");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}