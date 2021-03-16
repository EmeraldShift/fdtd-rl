#pragma once

#include "grid.hpp"
#include "phys.hpp"

#include <raft>

// Generate randomly-filled Grids with a given dimension (x, y, z).
class InitialGridGenerator : public raft::kernel {
	dim_t x, y, z;

public:
	CLONE();
	InitialGridGenerator(dim_t x, dim_t y, dim_t z);
	InitialGridGenerator(const InitialGridGenerator &o) : InitialGridGenerator(o.x, o.y, o.z) {}
	raft::kstatus run() final;
};

// Print the contents of a Grid, for debugging purposes.
class GridPrinter : public raft::kernel {
	bool silent;

public:
	GridPrinter(bool silent);
	raft::kstatus run() final;
};


class Worker : public raft::kernel {
protected:
	phys::params params;
	unsigned long iterations;
	bool initial = true;
	Grid grid;

public:
	Worker(phys::params params, unsigned long i);
};

class Hx : public Worker {
	bool print;
public:
	Hx(phys::params params, unsigned long i, bool print) :
		Worker(params, i), print(print) {}
	raft::kstatus run() final;
};

class Hy : public Worker {
public:
	Hy(phys::params params, unsigned long i) :
		Worker(params, i) {}
	raft::kstatus run() final;
};

class Hz : public Worker {
public:
	Hz(phys::params params, unsigned long i) :
		Worker(params, i) {}
	raft::kstatus run() final;
};

class Ex : public Worker {
public:
	Ex(phys::params params, unsigned long i) :
		Worker(params, i) {}
	raft::kstatus run() final;
};

class Ey : public Worker {
public:
	Ey(phys::params params, unsigned long i) :
		Worker(params, i) {}
	raft::kstatus run() final;
};

class Ez : public Worker {
public:
	Ez(phys::params params, unsigned long i) :
		Worker(params, i) {}
	raft::kstatus run() final;
};
