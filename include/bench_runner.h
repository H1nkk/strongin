#pragma once
#include <vector>
#include <iostream>
#include <chrono>
#include <filesystem>
#include "helper_structs.h"

namespace fs = std::filesystem;
using namespace std; // TODO убрать

void benchTimeTests(const std::vector<double (*)(double)>& funcs, info (*AGP)(double a, double b, double (*func)(double x))) {
	for (int i = funcs.size() - 1; i >= 0; i--) {
		double (*testingFunction)(double) = funcs[i];

		info res(0, 0, { 0, 0 }, 0);
		double minTimeSpent = INFINITY;
		for (int i = 0; i < TIMEMEASUREITERS; i++) {
			auto start = chrono::high_resolution_clock::now();
			res = AGP(leftBound[testingFunction], rightBound[testingFunction], testingFunction);
			auto stop = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
			double timeSpent = duration.count() / 1000000.0;
			minTimeSpent = min(minTimeSpent, timeSpent);
		}
		std::cout << "Func " << i + 1 << ". AGP result: " << res.extremumArg << ", actual result: " << extremums[funcs[i]] << '\n';
		std::cout << "Difference in results: " << fabs(res.extremumArg - extremums[funcs[i]]);
		std::cout << "\nIterations count : " << res.iterCount << "\n";
		std::cout << "Minimum calculating time : " << minTimeSpent << "\n";
		std::cout << "Left-closest-argument: " << res.closestArgs.first << "\n";
		std::cout << "Right-closest-argument: " << res.closestArgs.second << "\n";
		std::cout << '\n';
		std::cout << flush;

		string folderPath = "test-results";
		if (!fs::exists(folderPath)) {
			fs::create_directories(folderPath);
		}
		ofstream outfile("test-results/base-Function" + to_string(i + 1) + "-res.txt");
		outfile << "AGP-result: " << res.extremumArg << '\n';
		outfile << "Actual-result: " << extremums[funcs[i]] << '\n';
		outfile << "Difference-in-results: " << fabs(res.extremumArg - extremums[funcs[i]]) << '\n';
		outfile << "Iterations-count: " << res.iterCount << '\n';
		outfile << "Minimum-calculating-time: " << minTimeSpent << "\n";
		outfile << "Left-closest-argument: " << res.closestArgs.first << "\n";
		outfile << "Right-closest-argument: " << res.closestArgs.second << "\n";
		outfile << flush;
	}
}