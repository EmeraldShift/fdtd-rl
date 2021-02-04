#pragma once

#include "grid.hpp"
#include "phys.hpp"

#include <raft>

// Generate zero-filled Grids with a given dimension (x, y, z).
class InitialGridGenerator : public raft::kernel {
	dim_t x, y, z;

public:
	CLONE();
	InitialGridGenerator(dim_t x, dim_t y, dim_t z);
	InitialGridGenerator(const InitialGridGenerator &o) : InitialGridGenerator(o.x, o.y, o.z) {}
	raft::kstatus run() final;
};

// Generate zero-filled Grids with a given dimension (x, y, z).
class Loopback : public raft::kernel {
	bool initial = true;

public:
	Loopback();
	raft::kstatus run() final;
};

// Print the contents of a Grid, for debugging purposes.
class GridPrinter : public raft::kernel {
public:
	GridPrinter();
	raft::kstatus run() final;
};

class Hx : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Hx(phys::params params, unsigned long i);
	raft::kstatus run() final;
};

class Hy : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Hy(phys::params params, unsigned long i);
	raft::kstatus run() final;
};

class Hz : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Hz(phys::params params, unsigned long i);
	raft::kstatus run() final;
};

class Ex : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Ex(phys::params params, unsigned long i);
	raft::kstatus run() final;
};

class Ey : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Ey(phys::params params, unsigned long i);
	raft::kstatus run() final;
};

class Ez : public raft::kernel {
	phys::params params;
	unsigned long iterations;
	bool initial = true;
public:
	Ez(phys::params params, unsigned long i);
	raft::kstatus run() final;
};
