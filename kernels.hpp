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


class Worker : public raft::kernel {
protected:
	phys::params params;
	unsigned long iterations;
	Grid grid;

public:
	Worker(phys::params params, unsigned long i);
	void popGrids(Grid &a, Grid &b);
	void pushGrid();
};

class Hx : public Worker {
public:
	Hx(phys::params params, unsigned long i) :
		Worker(params, i) {}
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
