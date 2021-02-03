#pragma once

#include "grid.hpp"

#include <raft>

// Generate zero-filled Grids with a given dimension (x, y, z), n times.
class ZeroGridGenerator : public raft::kernel {
	dim_t x, y, z;
    unsigned long n;

public:
	ZeroGridGenerator(dim_t x, dim_t y, dim_t z, unsigned long n);
	raft::kstatus run() final;
};

// Print the contents of a Grid, for debugging purposes.
class GridPrinter : public raft::kernel {
public:
	GridPrinter();
	raft::kstatus run() final;
};

// TODO: Compute the H_x field
class Hx : public raft::kernel {
public:
	Hx();
	raft::kstatus run() final;
};
