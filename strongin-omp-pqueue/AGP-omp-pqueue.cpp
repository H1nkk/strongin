#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include "pqueue.h"

#include "../include/solver.h"

using namespace std;

struct dotInfo {
	double arg;
	double funcVal;
	double R;
};

struct RInfo {
	double R;
	double arg;
	RInfo(double nR = 0, double nArg = 0) : R(nR), arg(nArg) {}
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

info AGP(double a, double b, double (*func)(double x, int SLOWINGITERS), double r, double E, int ITERMAX, int SLOWINGITERS) {
	// èíèöèàëèçàöèÿ
	map<double, double> funcValue; // ìàïà èç àðãóìåíòà â çíà÷åíèå ôóíêöèè 
	double firstM = fabs((func(b, SLOWINGITERS) - func(a, SLOWINGITERS)) / (b - a));
	funcValue[a] = func(a, SLOWINGITERS);
	funcValue[b] = func(b, SLOWINGITERS);

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

	int threads_count = omp_get_max_threads();

	int iterations_done = 0;
	while (iterations_done < ITERMAX) {
		int threads_used = min(threads_count, (int)Rqueue.getSize()); // это число итераций которые будут сейчас выполнены в теле while
		vector<double> new_dot_vector(threads_used); // в i-м элементе хранится точка, полученная i-м потоком
		vector<double> new_func_value_vector(threads_used); // в i-м элементе хранится точка, полученная i-м потоком
		vector<double> max_R_vector(threads_used);
		vector<double> ldot_vector(threads_used);
		vector<double> rdot_vector(threads_used);
		vector<char> is_epsilon_achieved(threads_used, false); // char instead of vector because vector<bool> is a specification of vector and i dont know if its thread-safe

		int cur_thread = 0;
		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double cur_Rmax = Rqueue.get().R;
			max_R_vector[cur_thread] = cur_Rmax;
			ldot_vector[cur_thread] = Rqueue.get().arg;
			rdot_vector[cur_thread] = (*next(funcValue.find(ldot_vector[cur_thread]))).first;
			Rqueue.pop();
		}

#pragma omp parallel for
		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double Rmax = max_R_vector[cur_thread];
			double ldot = ldot_vector[cur_thread]; // левая граница подразбиваемого интервала
			double rdot = rdot_vector[cur_thread]; // правая граница подразбиваемого интервала
			double newDot = 0.5 * (rdot + ldot) - (funcValue[rdot] - funcValue[ldot]) * 0.5 / m;

			new_dot_vector[cur_thread] = newDot;

			new_func_value_vector[cur_thread] = func(newDot, SLOWINGITERS);
			if ((rdot - newDot) < E || (newDot - ldot) < E) {
				is_epsilon_achieved[cur_thread] = true;
			}
		}

		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			double newDot = new_dot_vector[cur_thread];
			funcValue[newDot] = new_func_value_vector[cur_thread];
			if (is_epsilon_achieved[cur_thread]) {
				break;
			}
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

		for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
			// Пересчет M для нового интервала
			double ldot = ldot_vector[cur_thread];
			double rdot = rdot_vector[cur_thread];

			double mdot = (*next(funcValue.find(ldot))).first;
			// rdot = (*next(funcValue.find(mdot))).first; // TODO проверить, можно ли это коментить вообще
			double Mcandidate1 = fabs((funcValue[mdot] - (funcValue[ldot])) / (mdot - ldot));
			double Mcandidate2 = fabs((funcValue[rdot] - (funcValue[mdot])) / (rdot - mdot));
			M = max({ M,Mcandidate1,Mcandidate2 });

			if (M > 0) {
				m = r * M;
			}
			else {
				m = 1;
			}
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
			for (cur_thread = 0; cur_thread < threads_used; cur_thread++) {
				double newDot = new_dot_vector[cur_thread];

				// TODO посмотреть, норм ли я тут заменил lArg и rArg (в сравнении с agp-map)
				double lArg = ldot_vector[cur_thread]; // аргумент, для которого будем пересчитывать R
				double mArg = newDot; // этот аргумент только что появился, для него нужно посчитать R
				double rArg = rdot_vector[cur_thread]; // правая граница нового интервала
			

				double newR1 = m * (mArg - lArg)
					+ (funcValue[mArg] - funcValue[lArg]) * (funcValue[mArg] - funcValue[lArg]) / (m * (mArg - lArg))
					- 2 * (funcValue[mArg] - funcValue[lArg]);
				double newR2 = m * (rArg - mArg)
					+ (funcValue[rArg] - funcValue[mArg]) * (funcValue[rArg] - funcValue[mArg]) / (m * (rArg - mArg))
					- 2 * (funcValue[rArg] - funcValue[mArg]);

				Rqueue.insert({ newR1, lArg });
				Rqueue.insert({ newR2, mArg });
			}
		}

		prevm = m;



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
	Solver solver(AGP, "omp-pqueue");
	solver.init();

	std::cout << std::fixed;

	solver.runBenchTests();

	return 0;
}
