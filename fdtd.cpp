#include "grid.hpp"
#include "kernels.hpp"

#include <iostream>
#include <cstdlib>

#include <raft>

int fdtd(dim_t dimX, dim_t dimY, dim_t dimZ, unsigned long t)
{
	ZeroGridGenerator zgg(dimX, dimY, dimZ, t);
	GridPrinter p;
	Hx hx;

	raft::map m;

	m += zgg >> hx["Hx"] >> p;

	m.exe();

	return EXIT_SUCCESS;
}
