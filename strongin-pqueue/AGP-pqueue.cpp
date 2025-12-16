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

using namespace std;
namespace fs = std::filesystem;
double r = 2.0; // method parameter
double E; // epsilon
int ITERMAX;
int TIMEMEASUREITERS;
int SLOWINGITERS;

map<double (*)(double), double> extremums;
map<double (*)(double), double> leftBound;
map<double (*)(double), double> rightBound;
vector<double (*)(double)> funcs;

struct info {
	double extremumArg; // значение точки экстремума
	double extremumVal; // значение функции в точке экстремума
	size_t iterCount; // число совершенных итераций
	info(double extremumArg, double  extremumVal, size_t iterCount) : extremumArg(extremumArg), extremumVal(extremumVal), iterCount(iterCount) {}
};

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

double funcSlower(double x) {
	double k = 1;
	for (int i = 0; i < SLOWINGITERS; i++) {
		k *= (cos(x) * cos(x) + sin(x) * sin(x));
	}
	return k;
}

double benchFunc1(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0);

	res *= k;
	return res;
}

double benchFunc2(double x) {
	double k = funcSlower(x);
	double res = 0;
	for (double k = 1.0; k <= 5.0; k += 1.0) {
		res += k * sin((k + 1) * x + k);
	}
	res *= -1;

	res *= k;
	return res;
}

double benchFunc3(double x) {
	double k = funcSlower(x);
	double res = (3.0 * x - 1.4) * sin(18.0 * x);

	res *= k;
	return res;
}

double benchFunc4(double x) {
	double k = funcSlower(x);
	double res = -(x + sin(x));
	res *= exp(-(x * x));

	res *= k;
	return res;
}

double benchFunc5(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double benchFunc6(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double benchFunc7(double x) {
	double k = funcSlower(x);
	double res = -sin(2 * numbers::pi_v<double> *x) * exp(-x);

	res *= k;
	return res;
}

double benchFunc8(double x) {
	double k = funcSlower(x);
	double res = (x * x - 5.0 * x + 6.0);
	res /= (x * x + 1.0);

	res *= k;
	return res;
}

double benchFunc9(double x) {
	double k = funcSlower(x);
	double res = -x + sin(3.0 * x) - 1.0;

	res *= k;
	return res;
}

double benchFunc10(double x) {
	double k = funcSlower(x);
	double res = 2.0 * (x - 3.0) * (x - 3.0) + exp(x * x * 0.5);

	res *= k;
	return res;
}

void init() {
	ifstream infile("../testing-properties.txt");
	string s;
	while (infile >> s)  {
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
		else {
			double foo;
			infile >> foo;
		}
	}

	extremums[benchFunc1] = 5.145735;
	leftBound[benchFunc1] = 2.7;
	rightBound[benchFunc1] = 7.5;
	funcs.push_back(benchFunc1);

	extremums[benchFunc2] = 5.791785; // тут несколько экстремумов
	leftBound[benchFunc2] = 0.0;
	rightBound[benchFunc2] = 10.0;
	funcs.push_back(benchFunc2);

	extremums[benchFunc3] = 0.96609;
	leftBound[benchFunc3] = 0;
	rightBound[benchFunc3] = 1.2;
	funcs.push_back(benchFunc3);

	extremums[benchFunc4] = 0.679560;
	leftBound[benchFunc4] = -10;
	rightBound[benchFunc4] = 10;
	funcs.push_back(benchFunc4);

	extremums[benchFunc5] = 5.19978;
	leftBound[benchFunc5] = 2.7;
	rightBound[benchFunc5] = 7.5;
	funcs.push_back(benchFunc5);

	extremums[benchFunc6] = 5.19978;
	leftBound[benchFunc6] = 2.7;
	rightBound[benchFunc6] = 7.5;
	funcs.push_back(benchFunc6);

	extremums[benchFunc7] = 0.224885;
	leftBound[benchFunc7] = 0;
	rightBound[benchFunc7] = 4;
	funcs.push_back(benchFunc7);

	extremums[benchFunc8] = 2.41420;
	leftBound[benchFunc8] = -5;
	rightBound[benchFunc8] = 5;
	funcs.push_back(benchFunc8);

	extremums[benchFunc9] = 5.877287;
	leftBound[benchFunc9] = 0;
	rightBound[benchFunc9] = 6.5;
	funcs.push_back(benchFunc9);

	extremums[benchFunc10] = 1.590700;
	leftBound[benchFunc10] = -3;
	rightBound[benchFunc10] = 3;
	funcs.push_back(benchFunc10);
}

info AGP(double a, double b, double (*func)(double x)) {
	// инициализация
	map<double, double> funcValue; // мапа из аргумента в значение функции 
	double firstM = fabs((func(b) - func(a)) / (b - a));
	funcValue[a] = func(a);
	funcValue[b] = func(b);

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
	firstR -= 2 * (rightFuncVal - leftFuncVal);
	Rqueue.insert({ firstR, a });

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {
		// Добавление новой точки
		RInfo maxInfo = Rqueue.get();
		double ldot = maxInfo.arg; // левая граница подразбиваемого интервала
		double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
		double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;
		funcValue[newDot] = func(newDot);

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
					- 2 * (rval - lval);
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
				- 2 * (funcValue[rArg] - funcValue[lArg]);

			Rqueue.pop();

			double newR1 = m * (mArg - lArg)
				+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
				- 2 * (funcValue[mArg] - funcValue[lArg]);
			double newR2 = m * (rArg - mArg)
				+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
				- 2 * (funcValue[rArg] - funcValue[mArg]);

			Rqueue.insert({ newR1, lArg });
			Rqueue.insert({ newR2, mArg });
		}

		prevm = m;
	}

	double extrArg = (*funcValue.begin()).first;
	double funcMin = funcValue[extrArg];
	for (auto p : funcValue) {
		if (p.second < funcMin) {
			funcMin = p.second;
			extrArg = p.first;
		}
	}
	 info res = { extrArg, funcMin, funcValue.size() - 2 };
	return res;
}

void benchTimeTests() {
	for (int i = funcs.size() - 1; i >= 0; i--) {
		double (*testingFunction)(double) = funcs[i];

		info res(0, 0, 0);
		double minTimeSpent = INFINITY;
		for (int i = 0; i < TIMEMEASUREITERS; i++) {
			auto start = chrono::high_resolution_clock::now();
			res = AGP(leftBound[testingFunction], rightBound[testingFunction], testingFunction);
			auto stop = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
			double timeSpent = duration.count() / 1000000.0;
			minTimeSpent = min(minTimeSpent, timeSpent);
		}
		cout << "Func " << i + 1 << ". AGP result: " << res.extremumArg << ", actual result: " << extremums[funcs[i]] << '\n';
		cout << "Difference in results: " << fabs(res.extremumArg - extremums[funcs[i]]);
		cout << "\nIterations count : " << res.iterCount << "\n";
		cout << "Minimum calculating time : " << minTimeSpent << "\n";
		cout << '\n';
		cout << flush;

		string folderPath = "test-results";
		if (!fs::exists(folderPath)) {
			fs::create_directories(folderPath);
		}
		ofstream outfile("test-results/pqueue-Function" + to_string(i + 1) + "-res.txt");
		outfile << "AGP-result: " << res.extremumArg << '\n';
		outfile << "Actual-result: " << extremums[funcs[i]] << '\n';
		outfile << "Difference-in-results: " << fabs(res.extremumArg - extremums[funcs[i]]) << '\n';
		outfile << "Iterations-count: " << res.iterCount << '\n';
		outfile << "Minimum-calculating-time: " << minTimeSpent << "\n";
		outfile << flush;
	}
}

int main() {
	init();

	cout << fixed;
	benchTimeTests();
	return 0;
}
