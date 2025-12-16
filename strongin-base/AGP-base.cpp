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
	vector<double> dots = { a,b };
	vector<double> value = { func(a), func(b) }; // value[i] = значение функции в точке i 
	vector<double> R(1);
	double M;
	double m;

	sort(dots.begin(), dots.end());

	int iteration;
	for (iteration = 1; iteration <= ITERMAX; iteration++) {

		int dotsCount = dots.size();

		for (int i = 0; i < dots.size(); i++) {
			value[i] = func(dots[i]);
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

	double extrArg = dots[0], funcMin = func(dots[0]);
	for (auto dot : dots) {
		if (func(dot) < funcMin) {
			funcMin = func(dot);
			extrArg = dot;
		}
	}
	info res = { extrArg, funcMin, dots.size() - 2 };
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
		ofstream outfile("test-results/base-Function" + to_string(i + 1) + "-res.txt");
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
