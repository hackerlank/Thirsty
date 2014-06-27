/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/Benchmark.h"
#include "core/Foreach.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

void fun() {
  static double x = 1;
  ++x;
  doNotOptimizeAway(x);
}
BENCHMARK(bmFun, n) { fun(); }
BENCHMARK(bmRepeatedFun, n) {
  FOR_EACH_RANGE (i, 0, n) {
    fun();
  }
}
BENCHMARK_DRAW_LINE()

BENCHMARK(gun, n) {
  static double x = 1;
  x *= 2000;
  doNotOptimizeAway(x);
}

BENCHMARK_DRAW_LINE()

BENCHMARK(baselinevector, n) {
  vector<int> v;

  BENCHMARK_SUSPEND {
    v.resize(1000);
  }

  FOR_EACH_RANGE (i, 0, 100) {
    v.push_back(42);
  }
}

BENCHMARK_RELATIVE(bmVector, n) {
  vector<int> v;
  FOR_EACH_RANGE (i, 0, 100) {
    v.resize(v.size() + 1, 42);
  }
}

BENCHMARK_DRAW_LINE()

BENCHMARK(superslow, n) {
  this_thread::sleep_for(chrono::milliseconds(1));
}

BENCHMARK_DRAW_LINE()

BENCHMARK(noMulti, n) {
  fun();
}

BENCHMARK_MULTI(multiSimple, n) {
  FOR_EACH_RANGE (i, 0, 10) {
    fun();
  }
  return 10;
}

BENCHMARK_RELATIVE_MULTI(multiSimpleRel, n) {
  FOR_EACH_RANGE (i, 0, 10) {
    fun();
    fun();
  }
  return 10;
}

BENCHMARK_MULTI(multiIterArgs, iter) {
  FOR_EACH_RANGE (i, 0, 10 * iter) {
    fun();
  }
  return 10 * iter;
}

BENCHMARK_RELATIVE_MULTI(multiIterArgsRel, iter) {
  FOR_EACH_RANGE (i, 0, 10 * iter) {
    fun();
    fun();
  }
  return 10 * iter;
}

unsigned paramMulti(unsigned iter, unsigned num) {
  for (unsigned i = 0; i < iter; ++i) {
    for (unsigned j = 0; j < num; ++j) {
      fun();
    }
  }
  return num * iter;
}

unsigned paramMultiRel(unsigned iter, unsigned num) {
  for (unsigned i = 0; i < iter; ++i) {
    for (unsigned j = 0; j < num; ++j) {
      fun();
      fun();
    }
  }
  return num * iter;
}

BENCHMARK_PARAM_MULTI(paramMulti, 1);
BENCHMARK_RELATIVE_PARAM_MULTI(paramMultiRel, 1);

BENCHMARK_PARAM_MULTI(paramMulti, 5);
BENCHMARK_RELATIVE_PARAM_MULTI(paramMultiRel, 5);
