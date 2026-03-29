#pragma once
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

#include "helper_structs.h"
#include "bench_functions.h"


namespace fs = std::filesystem;
using namespace std; // TODO убрать

class Solver {
	info (*solvingFunction)(double a, double b, double (*func)(double x, int), double, double, int, int);

	funcStats functionsStats;

	double r = 2.0; // method parameter
	double E; // epsilon

	int ITERMAX;
	int TIMEMEASUREITERS;
	int SLOWINGITERS;
	int THREADSNUM;

	bool isOmpUsed = false;

	string variantLabel;

public:
	Solver(info(*solvingFunction)(double a, double b, double (*func)(double x, int), double, double, int, int), std::string variantLabel, bool isOmpUsed = false) : solvingFunction(solvingFunction), variantLabel(variantLabel), isOmpUsed(isOmpUsed) {}
	void init() {
		ifstream infile("../testing-properties.txt");
		string s;
		while (infile >> s) {
			if (s == "ITERMAX") {
				infile >> ITERMAX;
			}
			else if (s == "TIMEMEASUREITERS") {
				infile >> TIMEMEASUREITERS;
			}
			else if (s == "SLOWINGITERS") {
				infile >> SLOWINGITERS;
			}
			else if (s == "EPSILON") {
				infile >> E;
			}
			else if (s == "THREADSNUM") {
				infile >> THREADSNUM;
			}
			else {
				double foo;
				infile >> foo;
			}
		}

		functionsStats.extremums[benchFunc1] = 5.145735;
		functionsStats.leftBound[benchFunc1] = 2.7;
		functionsStats.rightBound[benchFunc1] = 7.5;
		functionsStats.funcs.push_back(benchFunc1);

		functionsStats.extremums[benchFunc2] = 5.791785; // òóò íåñêîëüêî ýêñòðåìóìîâ
		functionsStats.leftBound[benchFunc2] = 0.0;
		functionsStats.rightBound[benchFunc2] = 10.0;
		functionsStats.funcs.push_back(benchFunc2);

		functionsStats.extremums[benchFunc3] = 0.96609;
		functionsStats.leftBound[benchFunc3] = 0;
		functionsStats.rightBound[benchFunc3] = 1.2;
		functionsStats.funcs.push_back(benchFunc3);

		functionsStats.extremums[benchFunc4] = 0.679560;
		functionsStats.leftBound[benchFunc4] = -10;
		functionsStats.rightBound[benchFunc4] = 10;
		functionsStats.funcs.push_back(benchFunc4);

		functionsStats.extremums[benchFunc5] = 5.19978;
		functionsStats.leftBound[benchFunc5] = 2.7;
		functionsStats.rightBound[benchFunc5] = 7.5;
		functionsStats.funcs.push_back(benchFunc5);

		functionsStats.extremums[benchFunc6] = 5.19978;
		functionsStats.leftBound[benchFunc6] = 2.7;
		functionsStats.rightBound[benchFunc6] = 7.5;
		functionsStats.funcs.push_back(benchFunc6);

		functionsStats.extremums[benchFunc7] = 0.224885;
		functionsStats.leftBound[benchFunc7] = 0;
		functionsStats.rightBound[benchFunc7] = 4;
		functionsStats.funcs.push_back(benchFunc7);

		functionsStats.extremums[benchFunc8] = 2.41420;
		functionsStats.leftBound[benchFunc8] = -5;
		functionsStats.rightBound[benchFunc8] = 5;
		functionsStats.funcs.push_back(benchFunc8);

		functionsStats.extremums[benchFunc9] = 5.877287;
		functionsStats.leftBound[benchFunc9] = 0;
		functionsStats.rightBound[benchFunc9] = 6.5;
		functionsStats.funcs.push_back(benchFunc9);

		functionsStats.extremums[benchFunc10] = 1.590700;
		functionsStats.leftBound[benchFunc10] = -3;
		functionsStats.rightBound[benchFunc10] = 3;
		functionsStats.funcs.push_back(benchFunc10);
	}

	/// @param a left interval boundary
	/// @param b right interval boundary
	/// @param func target function
	/// @param r method parameter
	/// @param E epsilon
	/// @param ITREMAX maximum iterations
	/// @returns optimization result
	info solve(double a, double b, double (*func)(double x, int)) {
		return solvingFunction(a, b, func, r, E, ITERMAX, SLOWINGITERS);
	}

	void runBenchTests() {
		if (isOmpUsed) {
			omp_set_num_threads(THREADSNUM);
		}
		auto& funcs = functionsStats.funcs;
		for (int i = functionsStats.funcs.size() - 1; i >= 0; i--) {
			double (*testingFunction)(double, int) = funcs[i];

			info res(0, 0, { 0, 0 }, 0);
			double minTimeSpent = INFINITY;
			for (int i = 0; i < TIMEMEASUREITERS; i++) {
				auto start = chrono::high_resolution_clock::now();
				res = solvingFunction(functionsStats.leftBound[testingFunction], functionsStats.rightBound[testingFunction], testingFunction, r, E, ITERMAX, SLOWINGITERS);
				auto stop = chrono::high_resolution_clock::now();
				auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
				double timeSpent = duration.count() / 1000000.0;
				minTimeSpent = min(minTimeSpent, timeSpent);
			}
			std::cout << "Func " << i + 1 << ". AGP result: " << res.extremumArg << ", actual result: " << functionsStats.extremums[funcs[i]] << '\n';
			std::cout << "Difference in results: " << fabs(res.extremumArg - functionsStats.extremums[funcs[i]]);
			std::cout << "\nIterations count : " << res.iterCount << "\n";
			std::cout << "Minimum calculating time : " << minTimeSpent << "\n";
			std::cout << "Smallest interval length: " << res.closestArgs.second - res.closestArgs.first << "\n";
			std::cout << '\n';
			std::cout << flush;

			string folderPath = "test-results";
			if (!fs::exists(folderPath)) {
				fs::create_directories(folderPath);
			}
			ofstream outfile("test-results/" + variantLabel + "-Function" + to_string(i + 1) + "-res.txt");
			outfile << "AGP-result: " << res.extremumArg << '\n';
			outfile << "Actual-result: " << functionsStats.extremums[funcs[i]] << '\n';
			outfile << "Difference-in-results: " << fabs(res.extremumArg - functionsStats.extremums[funcs[i]]) << '\n';
			outfile << "Iterations-count: " << res.iterCount << '\n';
			outfile << "Minimum-calculating-time: " << minTimeSpent << "\n";
			outfile << "Left-closest-argument: " << res.closestArgs.first << "\n";
			outfile << "Right-closest-argument: " << res.closestArgs.second << "\n";
			outfile << flush;
		}

		if (isOmpUsed) {
			omp_set_num_threads(omp_get_max_threads());
		}
	}
};