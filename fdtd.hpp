#pragma once

#include "grid.hpp"

constexpr unsigned long FLAG_QTH = 1;
constexpr unsigned long FLAG_DYN = 2;
constexpr unsigned long FLAG_VTL = 4;
constexpr unsigned long FLAG_SIL = 8;
constexpr unsigned long FLAG_PRT = 16;

int fdtd(dim_t, dim_t, dim_t, unsigned long, unsigned long);
