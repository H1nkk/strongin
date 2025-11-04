#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <numbers>
#include <set>
#include <chrono>
#include <vector>

using namespace std;

double r = 2.0; // method parameter
const double E = 1e-3; // epsilon
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
	int iterCount; // число совершенных итераций
	info(double extremumArg, double  extremumVal, int iterCount) : extremumArg(extremumArg), extremumVal(extremumVal), iterCount(iterCount) {}
};


double funcSlower(double x) {
	double k = 1;
	for (int i = 0; i < SLOWINGITERS; i++) {
		k *= (cos(x) * cos(x) + sin(x) * sin(x));
	}
	return k;
}

double becnhFunc1(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0);

	res *= k;
	return res;
}

double becnhFunc2(double x) {
	double k = funcSlower(x);
	double res = 0;
	for (double k = 1.0; k <= 5.0; k += 1.0) {
		res += k * sin((k + 1) * x + k);
	}
	res *= -1;

	res *= k;
	return res;
}

double becnhFunc3(double x) {
	double k = funcSlower(x);
	double res = (3.0 * x - 1.4) * sin(18.0 * x);

	res *= k;
	return res;
}

double becnhFunc4(double x) {
	double k = funcSlower(x);
	double res = -(x + sin(x));
	res *= exp(-(x * x));

	res *= k;
	return res;
}

double becnhFunc5(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double becnhFunc6(double x) {
	double k = funcSlower(x);
	double res = sin(x) + sin(10.0 * x / 3.0) + log(x) - 0.84 * x + 3.0;

	res *= k;
	return res;
}

double becnhFunc7(double x) {
	double k = funcSlower(x);
	double res = -sin(2 * numbers::pi_v<double> *x) * exp(-x);

	res *= k;
	return res;
}

double becnhFunc8(double x) {
	double k = funcSlower(x);
	double res = (x * x - 5.0 * x + 6.0);
	res /= (x * x + 1.0);

	res *= k;
	return res;
}

double becnhFunc9(double x) {
	double k = funcSlower(x);
	double res = -x + sin(3.0 * x) - 1.0;

	res *= k;
	return res;
}

double becnhFunc10(double x) {
	double k = funcSlower(x);
	double res = 2.0 * (x - 3.0) * (x - 3.0) + exp(x * x * 0.5);

	res *= k;
	return res;
}


void init() {
	ifstream infile("../testing-properties.txt");
	for (int consts = 0; consts < 3; consts++) {
		string s;
		infile >> s;
		if (s == "ITERMAX") {
			infile >> ITERMAX;
		}
		else if (s == "TIMEMEASUREITERS") {
			infile >> TIMEMEASUREITERS;
		}
		else if (s == "SLOWINGITERS") {
			infile >> SLOWINGITERS;
		}
	}

	extremums[becnhFunc1] = 5.145735;
	leftBound[becnhFunc1] = 2.7;
	rightBound[becnhFunc1] = 7.5;
	funcs.push_back(becnhFunc1);

	extremums[becnhFunc2] = 5.791785; // тут несколько экстремумов
	leftBound[becnhFunc2] = 0.0;
	rightBound[becnhFunc2] = 10.0;
	funcs.push_back(becnhFunc2);

	extremums[becnhFunc3] = 0.96609;
	leftBound[becnhFunc3] = 0;
	rightBound[becnhFunc3] = 1.2;
	funcs.push_back(becnhFunc3);

	extremums[becnhFunc4] = 0.679560;
	leftBound[becnhFunc4] = -10;
	rightBound[becnhFunc4] = 10;
	funcs.push_back(becnhFunc4);

	extremums[becnhFunc5] = 5.19978;
	leftBound[becnhFunc5] = 2.7;
	rightBound[becnhFunc5] = 7.5;
	funcs.push_back(becnhFunc5);

	extremums[becnhFunc6] = 5.19978;
	leftBound[becnhFunc6] = 2.7;
	rightBound[becnhFunc6] = 7.5;
	funcs.push_back(becnhFunc6);

	extremums[becnhFunc7] = 0.224885;
	leftBound[becnhFunc7] = 0;
	rightBound[becnhFunc7] = 4;
	funcs.push_back(becnhFunc7);

	extremums[becnhFunc8] = 2.41420;
	leftBound[becnhFunc8] = -5;
	rightBound[becnhFunc8] = 5;
	funcs.push_back(becnhFunc8);

	extremums[becnhFunc9] = 5.877287;
	leftBound[becnhFunc9] = 0;
	rightBound[becnhFunc9] = 6.5;
	funcs.push_back(becnhFunc9);

	extremums[becnhFunc10] = 1.590700;
	leftBound[becnhFunc10] = -3;
	rightBound[becnhFunc10] = 3;
	funcs.push_back(becnhFunc10);
}

info AGP(double a, double b, double (*func)(double x)) {
	vector<double> dots = { a,b };
	vector<double> value = { func(a), func(b) }; // value[i] = значение функции в точке i 
	vector<double> R(1);
	double M;
	double m;

	sort(dots.begin(), dots.end());

	int iteration;
	for (iteration = 0; iteration < ITERMAX; iteration++) {

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
	info res = { extrArg, funcMin, iteration };
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

		ofstream outfile("base-Function" + to_string(i + 1) + "-res.txt");
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
