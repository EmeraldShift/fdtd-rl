#pragma once

#include "grid.hpp"

#include <raft>

// Generate zero-filled Grids with a given dimension (x, y, z), n times.
class ZeroGridGenerator : public raft::kernel {
	dim_t x, y, z;
	int n;

public:
	ZeroGridGenerator(dim_t x, dim_t y, dim_t z, int n);
	virtual raft::kstatus run();
};

// Print the contents of a Grid, for debugging purposes.
class GridPrinter : public raft::kernel {
public:
	GridPrinter();
	virtual raft::kstatus run();
};

// TODO: Compute the H_x field
class Hx : public raft::kernel {
public:
	Hx();
	virtual raft::kstatus run();
};
