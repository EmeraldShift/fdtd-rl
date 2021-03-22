#include "kernels.hpp"

#include <cstdlib>
#include <iostream>

template class Worker<WorkerType::H>;
template class Worker<WorkerType::E>;

GridPrinter::GridPrinter(phys::params params, bool silent) : params(params), silent(silent)
{
	input.addPort<elem_t>("grid");
}

raft::kstatus GridPrinter::run()
{;
	if (silent)
		return raft::stop;

	Grid g = Grid(params.nx, params.ny, params.nz);
	for (dim_t i = 0, lim = g.x() * g.y() * g.z(); i < lim; i++)
		input["grid"].pop(g[i]);

	std::stringstream ss;
	ss << "Grid(" << g.x() << ", " << g.y() << ", " << g.z() << ") = [ ";
	for (dim_t i = 0; i < g.x() * g.y() * g.z(); i++)
		ss << g[i] << " ";
	ss << "]" << std::endl;

	std::cout << ss.rdbuf() << std::endl;
	return raft::proceed;
}

template <WorkerType T>
Worker<T>::Worker(phys::params params, unsigned long i, bool silent) :
	params(params), iterations(i), silent(silent), grid(params.nx, params.ny, params.nz) {
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

template <WorkerType T>
void Worker<T>::popGrids(Grid &a, Grid &b)
{
	a = Grid(params.nx, params.ny, params.nz);
	b = Grid(params.nx, params.ny, params.nz);
	for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++) {
		input["A"].pop(a[i]);
		input["B"].pop(b[i]);
	}
}

template <WorkerType T>
void Worker<T>::pushGrid()
{
	for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++) {
		output["out_A"].push(grid[i]);
		output["out_B"].push(grid[i]);
	}
}

// H-field workers first pop grids, then compute, then push.
template <>
raft::kstatus Worker<WorkerType::H>::run()
{
	if (iterations-- == 0) {
		if (!silent) {
			for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
				output["final"].push(grid[i]);
		}
		return raft::stop;
	}

	Grid a, b;
	popGrids(a, b);
	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				grid.at(x, y, z) += diff(a, b, x, y, z);
			}
		}
	}

	pushGrid();
	return raft::proceed;
}


// E-field workers first push, then pop grids, then compute.
template <>
raft::kstatus Worker<WorkerType::E>::run()
{
	if (iterations-- == 0) {
		if (!silent) {
			for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
				output["final"].push(grid[i]);
		}
		return raft::stop;
	}

	pushGrid();

	Grid a, b;
	popGrids(a, b);
	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				grid.at(x, y, z) -= diff(a, b, x, y, z);
			}
		}
	}
	return raft::proceed;
}

elem_t Hx::diff(const Grid &ey, const Grid &ez, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ey.at(x,y,z+1)-ey.at(x,y,z))*params.cz - (ez.at(x,y+1,z)-ez.at(x,y,z))*params.cy);
}

elem_t Hy::diff(const Grid &ez, const Grid &ex, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ez.at(x+1,y,z)-ez.at(x,y,z))*params.cx - (ex.at(x,y,z+1)-ex.at(x,y,z))*params.cz);
}

elem_t Hz::diff(const Grid &ex, const Grid &ey, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ex.at(x,y+1,z)-ex.at(x,y,z))*params.cy - (ey.at(x+1,y,z)-ey.at(x,y,z))*params.cx);
}

elem_t Ex::diff(const Grid &hy, const Grid &hz, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hy.at(x,y,z)-hy.at(x,y,z-1))*params.cz - (hz.at(x,y,z)-hz.at(x,y-1,z))*params.cy);
}

elem_t Ey::diff(const Grid &hz, const Grid &hx, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hz.at(x,y,z)-hz.at(x-1,y,z))*params.cx - (hx.at(x,y,z)-hx.at(x,y,z-1))*params.cz);
}

elem_t Ez::diff(const Grid &hx, const Grid &hy, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hx.at(x,y,z)-hx.at(x,y-1,z))*params.cy - (hy.at(x,y,z)-hy.at(x-1,y,z))*params.cx);
}
