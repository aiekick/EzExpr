
/*
MIT License

Copyright (c) 2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <EzExpr/perfos/Test_Expr_Perfos.h>
#include <EzExpr.hpp>
#include <functional>
#include <iostream>
#include <cassert>
#include <chrono>
#include <fstream>

static bool Bench(const std::string& vEzExpr,
                  const std::function<double(ez::Expr& vEv)>& vEzExprIteration,
                  const std::function<double()>& vCppExpr,
                  int vIterations,
                  double slowdownThreshold,
                  double& vOutEzExprTotalTime,
                  double& vOutEzExprAvgTime,
                  double& vOutCppTotalTime,
                  double& vOutCppAvgTime,
                  std::ostream* outputStreamPtr) {
    try {
        assert(!vEzExpr.empty());
        ez::Expr ev;
        ev.parse(vEzExpr);

        // Accumulation des résultats
        double ezExprAccumulation = 0.0;
        double cppExprAccumulation = 0.0;

        // Mesure du temps pour EzExpr
        auto start_ez = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < vIterations; ++i) {
            ezExprAccumulation += vEzExprIteration(ev); 
        }
        auto end_ez = std::chrono::high_resolution_clock::now();
        vOutEzExprTotalTime = std::chrono::duration<double, std::milli>(end_ez - start_ez).count();
     //   vOutEzExprAvgTime = vOutEzExprTotalTime / vIterations;

        // Mesure du temps pour l'évaluation en C++
        auto start_cpp = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < vIterations; ++i) {
            cppExprAccumulation += vCppExpr();  
        }
        auto end_cpp = std::chrono::high_resolution_clock::now();
        vOutCppTotalTime = std::chrono::duration<double, std::milli>(end_cpp - start_cpp).count();
    //    vOutCppAvgTime = vOutCppTotalTime / vIterations;

        // Calcul du pourcentage de ralentissement
        double slowdown_total_percentage = ((vOutEzExprTotalTime / vOutCppTotalTime) - 1.0) * 100.0;
      //  double slowdown_avg_percentage = ((vOutEzExprAvgTime / vOutCppAvgTime) - 1.0) * 100.0;

        // Écriture des résultats dans le flux sous forme de tableau Markdown
        if (outputStreamPtr != nullptr) {
            (*outputStreamPtr) <<                         //
                "| " << vEzExpr <<                        //
                " | " << vOutEzExprTotalTime <<           //
                " | " << vOutCppTotalTime <<           //
                " | " << slowdown_total_percentage <<  //
                "% |\n";
        }

        // Vérification si le ralentissement dépasse le seuil
        if (slowdown_total_percentage > slowdownThreshold) { 
            return false; 
        }

        // Comparaison des résultats accumulés
        if (std::abs(ezExprAccumulation - cppExprAccumulation) > std::numeric_limits<double>::epsilon()) {
            if (outputStreamPtr != nullptr) {  //
                (*outputStreamPtr) << "| " <<  //
                    vEzExpr << " | Accumulation mismatch |" << std::endl;
            }
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////
//// PERFOS ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool Test_Expr_Perfo_x_squared(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "x*x",
        [](ez::Expr& ev) {
            static double x = 1.0;
            double result = ev.set("x", x).eval().getResult();
            x += 0.1; 
            return result;
        },
        []() {
            static double x = 1.0;
            double result = x * x;
            x += 0.1;  
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_x_cubed(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "x*x*x",
        [](ez::Expr& ev) {
            static double x = 1.0;
            double result = ev.set("x", x).eval().getResult();
            x += 0.1;
            return result;
        },
        []() {
            static double x = 1.0;
            double result = x * x * x;
            x += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_sin_x(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sin(x)",
        [](ez::Expr& ev) {
            static double x = 1.0;
            double result = ev.set("x", x).eval().getResult();
            x += 0.1;
            return result;
        },
        []() {
            static double x = 1.0;
            double result = std::sin(x);
            x += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_sqrt_x(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sqrt(x)",
        [](ez::Expr& ev) {
            static double x = 1.0;
            double result = ev.set("x", x).eval().getResult();
            x += 0.1;
            return result;
        },
        []() {
            static double x = 1.0;
            double result = std::sqrt(x);
            x += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_cos_x_sin_y(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "cos(x) * sin(y)",
        [](ez::Expr& ev) {
            static double x = 1.0;
            static double y = 1.0;
            double result = ev.set("x", x).set("y", y).eval().getResult();
            x += 0.1;
            y += 0.1;
            return result;
        },
        []() {
            static double x = 1.0;
            static double y = 1.0;
            double result = std::cos(x) * std::sin(y);
            x += 0.1;
            y += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "a+5",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = a + 5.0;
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_5_plus_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "5+a+5",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = 5.0 + a + 5.0;
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_abs_a_plus_5(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "abs(a+5)",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = std::abs(a + 5.0);
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_a_plus_5_times_2(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "(a+5)*2",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = (a + 5.0) * 2.0;
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_a_plus_5_times_2_alt(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "a+(5*2)",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = a + (5.0 * 2.0);
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_sqrt_pow_a(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "sqrt(pow(a, 1.5) + pow(a, 2.5))",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = std::sqrt(std::pow(a, 1.5) + std::pow(a, 2.5));
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_complex_fraction(double slowdownThreshold, int iterations, std::ostream* outputStreamPtr) {
    double ez_expr_total_time, ez_expr_avg_time;
    double cpp_expr_total_time, cpp_expr_avg_time;

    return Bench(
        "(1/(a+1)+2/(a+2)+3/(a+3))",
        [](ez::Expr& ev) {
            static double a = 1.0;
            double result = ev.set("a", a).eval().getResult();
            a += 0.1;
            return result;
        },
        []() {
            static double a = 1.0;
            double result = (1.0 / (a + 1.0)) + (2.0 / (a + 2.0)) + (3.0 / (a + 3.0));
            a += 0.1;
            return result;
        },
        iterations,
        slowdownThreshold,
        ez_expr_total_time,
        ez_expr_avg_time,
        cpp_expr_total_time,
        cpp_expr_avg_time,
        outputStreamPtr);
}

bool Test_Expr_Perfo_All(double slowdownThreshold, int iterations) {
    bool res = true;

    // Ouverture d'un fichier pour enregistrer les résultats
    std::ofstream resultsFile(PROJECT_PATH "/benchmark_results.md");

    if (resultsFile.is_open()) {
        // Imprimer l'en-tête du tableau
        resultsFile << "| Expression | EzExpr Total Time (ms) | C++ Total Time (ms) | Slowdown (%) |\n";
        resultsFile << "|------------|------------------|-----------------|----------------|--------------|---------------------|-------------------|\n";

        // Exécution des tests
        res &= Test_Expr_Perfo_x_squared(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_x_cubed(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_sin_x(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_sqrt_x(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_cos_x_sin_y(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_5_plus_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_abs_a_plus_5(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_a_plus_5_times_2(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_a_plus_5_times_2_alt(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_sqrt_pow_a(slowdownThreshold, iterations, &resultsFile);
        res &= Test_Expr_Perfo_complex_fraction(slowdownThreshold, iterations, &resultsFile);

        resultsFile.close();  // Fermer le fichier après les tests
    } else {
        std::cerr << "Failed to open results file." << std::endl;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v) \
    if (vTest == std::string(#v)) return v(slowdownThreshold, iterations)

#define IfTestBenchExist(v) \
    if (vTest == std::string(#v)) return v(slowdownThreshold, iterations, nullptr)

bool Test_Expr_Perfos_run_test(const std::string& vTest) {
    double slowdownThreshold = 5000.0;  // 50% de ralentissement maximum acceptable
    int iterations = 10000;
    // Parsing
    IfTestExist(Test_Expr_Perfo_All);
    else IfTestBenchExist(Test_Expr_Perfo_x_squared);
    else IfTestBenchExist(Test_Expr_Perfo_x_cubed);
    else IfTestBenchExist(Test_Expr_Perfo_sin_x);
    else IfTestBenchExist(Test_Expr_Perfo_sqrt_x);
    else IfTestBenchExist(Test_Expr_Perfo_cos_x_sin_y);
    else IfTestBenchExist(Test_Expr_Perfo_a_plus_5);
    else IfTestBenchExist(Test_Expr_Perfo_5_plus_a_plus_5);
    else IfTestBenchExist(Test_Expr_Perfo_abs_a_plus_5);
    else IfTestBenchExist(Test_Expr_Perfo_a_plus_5_times_2);
    else IfTestBenchExist(Test_Expr_Perfo_a_plus_5_times_2_alt);
    else IfTestBenchExist(Test_Expr_Perfo_sqrt_pow_a);
    else IfTestBenchExist(Test_Expr_Perfo_complex_fraction);
    // default
    return false;
}