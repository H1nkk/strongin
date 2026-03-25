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
#include <omp.h>

using namespace std;
namespace fs = std::filesystem;

double r = 2.0; // method parameter
double E; // epsilon
int ITERMAX;
int TIMEMEASUREITERS;
int SLOWINGITERS;
int THREADS_NUM;

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
	double funcValue;
	dotInfo(double nfuncValue = 0.0) : funcValue(nfuncValue) {}
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
		else if (s == "THREADS_NUM") {
			infile >> THREADS_NUM;
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

	omp_set_num_threads(THREADS_NUM);
}

info custom_AGP(double a, double b, double (*func)(double x)) {
	// инициализация
	map<double, double> funcValue; // мапа из аргумента в значение функции 
	double firstM = fabs((func(b) - func(a)) / (b - a));
	funcValue[a] = func(a);
	funcValue[b] = func(b);

	multimap<double, double> RtoArg; // в RtoArg[curR] хранится аргумент x, с которого начинается отрезок для характеристикой R = curR
	double firstR = (b - a);
	RtoArg.insert(make_pair(firstR, a));

	// int threads_count = omp_get_max_threads();
	int threads_count = 1;

	int iterations_done = 0;
	while (iterations_done < ITERMAX) {
		int threads_used = min(threads_count, (int)RtoArg.size()); // это число итераций которые будут сейчас выполнены в теле while
		vector<double> new_dot_vector(threads_used); // в i-м элементе хранится точка, полученная i-м потоком
		vector<double> new_func_value_vector(threads_used); // в i-м элементе хранится точка, полученная i-м потоком
		vector<double> max_R_vector(threads_used);
		vector<double> ldot_vector(threads_used);
		vector<double> rdot_vector(threads_used);
		vector<bool> is_epsilon_achieved(threads_used, false);

		int cur_thread = 0;
		for (auto it = RtoArg.rbegin(); it != RtoArg.rend(); it++) {
			double key = (*it).first;
			max_R_vector[cur_thread] = key;
			cur_thread++;
			if (cur_thread >= threads_used)
				break;
		}
		if (iterations_done == 39) {
			cout << "";
		}

// #pragma omp parallel for
		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double Rmax = max_R_vector[cur_thread];
			double ldot = RtoArg.find(Rmax)->second; // левая граница подразбиваемого интервала
			double rdot = (*next(funcValue.find(ldot))).first; // правая граница подразбиваемого интервала
			double newDot = 0.5 * (rdot + ldot);

			new_dot_vector[cur_thread] = newDot;
			ldot_vector[cur_thread] = ldot;
			rdot_vector[cur_thread] = rdot;

			new_func_value_vector[cur_thread] = 0;

			if ((rdot - newDot) < E || (newDot - ldot) < E) {
				is_epsilon_achieved[cur_thread] = true;
				break;
			}
		}


		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double newDot = new_dot_vector[cur_thread];
			funcValue[newDot] = new_func_value_vector[cur_thread];
		}

		
		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double Rmax = max_R_vector[cur_thread];
			double newDot = new_dot_vector[cur_thread];

			double lArg = ldot_vector[cur_thread]; // аргумент, для которого будем пересчитывать R
			double mArg = newDot; // этот аргумент только что появился, для него нужно посчитать R
			double rArg = rdot_vector[cur_thread]; // правая граница нового интервала
			double RToRecalculate1 = rArg - lArg;

			RtoArg.erase(RtoArg.find(RToRecalculate1));

			double newR1 = (mArg - lArg);
			double newR2 = (rArg - mArg);

			RtoArg.insert(make_pair(newR1, lArg));
			RtoArg.insert(make_pair(newR2, mArg));
		}
		

		iterations_done += threads_used;

		bool done = false;
		for (auto x : is_epsilon_achieved) {
			if (x == true) {
				done = true;
				break;
			}
		}
		if (done) {
			break;
		}
	}

	vector<double> args;
	for (auto [arg, val] : funcValue) {
		args.push_back(arg);
	}
	vector<double> func_values_vec(args.size());
	assert(args.size() == iterations_done + 2);


#pragma omp parallel for
	for (int i = 0; i < iterations_done + 2; i++) {
		func_values_vec[i] = func(args[i]);
	}

	for (int i = 0; i < iterations_done + 2; i++) {
		funcValue[args[i]] = func_values_vec[i];
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
			res = custom_AGP(leftBound[testingFunction], rightBound[testingFunction], testingFunction);
			auto stop = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
			double timeSpent = duration.count() / 1000000.0;
			minTimeSpent = min(minTimeSpent, timeSpent);
		}
		cout << "Func " << i + 1 << ". custom AGP result: " << res.extremumArg << ", actual result: " << extremums[funcs[i]] << '\n';
		cout << "Difference in results: " << fabs(res.extremumArg - extremums[funcs[i]]);
		cout << "\nIterations count : " << res.iterCount << "\n";
		cout << "Minimum calculating time : " << minTimeSpent << "\n";
		cout << '\n';
		cout << flush;

		string folderPath = "test-results";
		if (!fs::exists(folderPath)) {
			fs::create_directories(folderPath);
		}
		ofstream outfile("test-results/omp-simple-Function" + to_string(i + 1) + "-res.txt");
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
