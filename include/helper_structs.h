#pragma once
#include <utility>
#include <map>

struct info {
	double extremumArg; // значение точки экстремума
	double extremumVal; // значение функции в точке экстремума
	std::pair<double, double> closestArgs; // две ближайшие точки (из-за которых произошёл выход по точности, E)
	size_t iterCount; // число совершенных итераций
	info(double extremumArg, double  extremumVal, std::pair<double, double> closestArgs, size_t iterCount) : extremumArg(extremumArg), extremumVal(extremumVal), closestArgs(closestArgs), iterCount(iterCount) {}
};

struct funcStats {
	std::map<double (*)(double), double> extremums;
	std::map<double (*)(double), double> leftBound;
	std::map<double (*)(double), double> rightBound;
	std::vector<double (*)(double)> funcs;
};
