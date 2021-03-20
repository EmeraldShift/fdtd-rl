#include "kernels.hpp"

#include <cstdlib>
#include <iostream>

GridPrinter::GridPrinter(phys::params params, bool silent) : params(params), silent(silent)
{
	input.addPort<elem_t>("grid");
}

raft::kstatus GridPrinter::run()
{
	Grid g = Grid(params.nx, params.ny, params.nz);
	for (dim_t i = 0, lim = g.x() * g.y() * g.z(); i < lim; i++)
		input["grid"].pop(g[i]);
	if (silent)
		return raft::proceed;

	std::stringstream ss;
	ss << "Grid(" << g.x() << ", " << g.y() << ", " << g.z() << ") = [ ";
	for (dim_t i = 0; i < g.x() * g.y() * g.z(); i++)
		ss << g[i] << " ";
	ss << "]" << std::endl;

	std::cout << ss.rdbuf() << std::endl;
	return raft::proceed;
}

Worker::Worker(phys::params params, unsigned long i) :
	params(params), iterations(i), grid(params.nx, params.ny, params.nz) {
	input.addPort<int>("dummy");
	input.addPort<elem_t>("A");
	input.addPort<elem_t>("B");
	output.addPort<elem_t>("out_A");
	output.addPort<elem_t>("out_B");
	output.addPort<elem_t>("final");

	// Random initial grid
	for (dim_t j = 0, lim = params.nx * params.ny * params.nz; j < lim; j++) {
		grid[j] = drand48();
	}
}

void Worker::popGrids(Grid &a, Grid &b)
{
	a = Grid(params.nx, params.ny, params.nz);
	b = Grid(params.nx, params.ny, params.nz);
	for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++) {
		input["A"].pop(a[i]);
		input["B"].pop(b[i]);
	}
}

void Worker::pushGrid()
{
	for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++) {
		output["out_A"].push(grid[i]);
		output["out_B"].push(grid[i]);
	}
}

raft::kstatus Hx::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	Grid ey, ez;
	popGrids(ey, ez);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				grid.at(x, y, z) += params.ch * ((ey.at(x,y,z+1)-ey.at(x,y,z))*params.cz - (ez.at(x,y+1,z)-ez.at(x,y,z))*params.cy);
			}
		}
	}

	pushGrid();
	return raft::proceed;
}

raft::kstatus Hy::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	Grid ez, ex;
	popGrids(ez, ex);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				grid.at(x, y, z) += params.ch * ((ez.at(x+1,y,z)-ez.at(x,y,z))*params.cx - (ex.at(x,y,z+1)-ex.at(x,y,z))*params.cz);
			}
		}
	}

	pushGrid();
	return raft::proceed;
}

raft::kstatus Hz::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	Grid ex, ey;
	popGrids(ex, ey);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				grid.at(x, y, z) += params.ch * ((ex.at(x,y+1,z)-ex.at(x,y,z))*params.cy - (ey.at(x+1,y,z)-ey.at(x,y,z))*params.cx);
			}
		}
	}

	pushGrid();
	return raft::proceed;
}

raft::kstatus Ex::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	pushGrid();

	Grid hy, hz;
	popGrids(hy, hz);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				grid.at(x, y, z) -= params.ce * ((hy.at(x,y,z)-hy.at(x,y,z-1))*params.cz - (hz.at(x,y,z)-hz.at(x,y-1,z))*params.cy);
			}
		}
	}
	return raft::proceed;
}

raft::kstatus Ey::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	pushGrid();

	Grid hz, hx;
	popGrids(hz, hx);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				grid.at(x, y, z) -= params.ce * ((hz.at(x,y,z)-hz.at(x-1,y,z))*params.cx - (hx.at(x,y,z)-hx.at(x,y,z-1))*params.cz);
			}
		}
	}
	return raft::proceed;
}

raft::kstatus Ez::run()
{
	if (iterations-- == 0) {
		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
			output["final"].push(grid[i]);
		return raft::stop;
	}

	pushGrid();

	Grid hx, hy;
	popGrids(hx, hy);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				grid.at(x, y, z) -= params.ce * ((hx.at(x,y,z)-hx.at(x,y-1,z))*params.cy - (hy.at(x,y,z)-hy.at(x-1,y,z))*params.cx);
			}
		}
	}
	return raft::proceed;
}
