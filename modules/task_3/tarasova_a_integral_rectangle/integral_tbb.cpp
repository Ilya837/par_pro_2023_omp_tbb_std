// Copyright 2023 Tarasova Anastasiya
#include <tbb/tbb.h>
#include <vector>
#include "../../../modules/task_3/tarasova_a_integral_rectangle/integral_tbb.h"

class CalculationXY {
 private:
    double my_h;
    double my_a;
    std::vector<double>* my_x1;
 public:
    void operator()(const tbb::blocked_range<size_t>& r) const {
        for (size_t i = r.begin(); i < r.end(); i++)
            (*my_x1)[i] = my_a + i * my_h + my_h / 2;
    }
    CalculationXY(const double& h, const double& a, std::vector<double>* x1) {
        my_h = h;
        my_a = a;
        my_x1 = x1;
    }
};

class Sum {
 private:
    double my_x;
    double my_y;
    double my_a;
    double* my_sum;
    double my_h;
    double (*my_f)(double, double, double);
 public:
    void operator()(const tbb::blocked_range<size_t>& r) const {
        for (size_t i = r.begin(); i < r.end(); i++) {
            double z = my_a + i * my_h + my_h / 2;
            *my_sum += my_f(my_x, my_y, z) * my_h * my_h * my_h;
        }
    }
    Sum(const double& x, const double& y, const double& a, const double& h,
double* sum, double f(double, double, double)) {
        my_x = x;
        my_y = y;
        my_a = a;
        my_sum = sum;
        my_h = h;
        my_f = (*f);
    }
};


double getParallel(const double& a1, const double& a2, const double& a3, const double& b1,
    const double& b2, const double& b3, const double& h, double f(double, double, double)) {
    double sum = 0.0, x, y, z;
    int n1, n2, n3;
    n1 = static_cast<int>((b1 - a1) / h);
    n2 = static_cast<int>((b2 - a2) / h);
    n3 = static_cast<int>((b3 - a3) / h);
    std::vector<double> x1(n1), y1(n2), z1(n3);
    tbb::parallel_for(tbb::blocked_range<size_t>(0, n1), CalculationXY(h, a1, &x1));
    tbb::parallel_for(tbb::blocked_range<size_t>(0, n2), CalculationXY(h, a2, &y1));
    for (int i = 0; i < n1; i++)
        for (int j = 0; j < n2; j++)
            tbb::parallel_for(tbb::blocked_range<size_t>(0, n3), Sum(x1[i], y1[j], a3, h, &sum, f));
    return sum;
}

double getSequential(const double& a1, const double& a2, const double& a3, const double& b1,
    const double& b2, const double& b3, const double& h, double f(double, double, double)) {
    double sum = 0.0, x, y, z;
    int n1, n2, n3;
    n1 = static_cast<int>((b1 - a1) / h);
    n2 = static_cast<int>((b2 - a2) / h);
    n3 = static_cast<int>((b3 - a3) / h);
    std::vector<double> x1(n1), y1(n2), z1(n3);
    for (int i = 0; i < n1; i++) {
        x = a1 + i * h + h / 2;
        x1[i] = x;
    }
    for (int i = 0; i < n2; i++) {
        y = a2 + i * h + h / 2;
        y1[i] = y;
    }
    for (int i = 0; i < n3; i++) {
        z = a3 + i * h + h / 2;
        z1[i] = z;
    }
    for (int i = 0; i < n1; i++)
        for (int j = 0; j < n2; j++)
            for (int k = 0; k < n3; k++)
                sum += f(x1[i], y1[j], z1[k]) * h * h * h;
    return sum;
}
