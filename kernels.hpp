#pragma once

#include "grid.hpp"
#include "phys.hpp"

#include <raft>
#include <iostream>

// RaftLib flops over when the entire map is cyclic.
// Having a "dummy" kernel with outbound edges only
// allows us to trick RL into working properly here.
class DummyKernel : public raft::kernel {
public:
	CLONE();
	DummyKernel() : raft::kernel() {
		output.addPort<int>("dummy");
	}
	DummyKernel(const DummyKernel&) : DummyKernel() {}
	raft::kstatus run() final {
		output["dummy"].push(5);
		return raft::stop;
	}
};

// Print the contents of a Grid, for debugging purposes.
class GridPrinter : public raft::kernel {
	phys::params params;
	bool silent;

public:
	explicit GridPrinter(phys::params params, bool silent);
	raft::kstatus run() final;
};


enum class WorkerType { H, E };

template <WorkerType T>
class Worker : public raft::kernel {
protected:
	phys::params params;
	unsigned long iterations;
	bool silent;
	Grid grid;

public:
	Worker(phys::params params, unsigned long i, bool silent);
	void popGrids(Grid &a, Grid &b);
	void pushGrid();

	raft::kstatus run() final;
	virtual elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z) = 0;
};

class Hx : public Worker<WorkerType::H> {
public:
	Hx(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};

class Hy : public Worker<WorkerType::H> {
public:
	Hy(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};

class Hz : public Worker<WorkerType::H> {
public:
	Hz(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};

class Ex : public Worker<WorkerType::E> {
public:
	Ex(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};

class Ey : public Worker<WorkerType::E> {
public:
	Ey(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};

class Ez : public Worker<WorkerType::E> {
public:
	Ez(phys::params params, unsigned long i, bool silent) :
		Worker(params, i, silent) {}
	elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z ) final;
};
