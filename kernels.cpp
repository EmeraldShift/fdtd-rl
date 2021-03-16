#include "kernels.hpp"

#include <cstdlib>
#include <iostream>

GridPrinter::GridPrinter(bool silent) : silent(silent)
{
	input.addPort<Grid>("grid");
}

raft::kstatus GridPrinter::run()
{
	Grid g;
	input["grid"].pop(g);
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
	input.addPort<Grid>("dummy");
	input.addPort<Grid>("A");
	input.addPort<Grid>("B");
	output.addPort<Grid>("out_A");
	output.addPort<Grid>("out_B");
	output.addPort<Grid>("Final");

	// Random initial grid
	for (dim_t j = 0, lim = params.nx * params.ny * params.nz; j < lim; j++) {
		grid[j] = drand48();
	}
}

raft::kstatus Hx::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	Grid ey, ez;
	input["A"].pop(ey);
	input["B"].pop(ez);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) += params.ch * ((ey.at(x,y,z+1)-ey.at(x,y,z))*params.cz - (ez.at(x,y+1,z)-ez.at(x,y,z))*params.cy);
			}
		}
	}

	if (print)
		std::cout << "Hx: " << iterations << " Iterations remaining." << std::endl;

	output["out_A"].push(grid);
	output["out_B"].push(grid);
	return raft::proceed;
}

raft::kstatus Hy::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	Grid ez, ex;
	input["A"].pop(ez);
	input["B"].pop(ex);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) += params.ch * ((ez.at(x+1,y,z)-ez.at(x,y,z))*params.cx - (ex.at(x,y,z+1)-ex.at(x,y,z))*params.cz);
			}
		}
	}

	output["out_A"].push(grid);
	output["out_B"].push(grid);
	return raft::proceed;
}

raft::kstatus Hz::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	Grid ex, ey;
	input["A"].pop(ex);
	input["B"].pop(ey);

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) += params.ch * ((ex.at(x,y+1,z)-ex.at(x,y,z))*params.cy - (ey.at(x+1,y,z)-ey.at(x,y,z))*params.cx);
			}
		}
	}

	output["out_A"].push(grid);
	output["out_B"].push(grid);
	return raft::proceed;
}

raft::kstatus Ex::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	output["out_A"].push(grid);
	output["out_B"].push(grid);

	Grid hy, hz;
	input["A"].pop(hy);
	input["B"].pop(hz);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) -= params.ce * ((hy.at(x,y,z)-hy.at(x,y,z-1))*params.cz - (hz.at(x,y,z)-hz.at(x,y-1,z))*params.cy);
			}
		}
	}
	return raft::proceed;
}

raft::kstatus Ey::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	output["out_A"].push(grid);
	output["out_B"].push(grid);

	Grid hz, hx;
	input["A"].pop(hz);
	input["B"].pop(hx);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) -= params.ce * ((hz.at(x,y,z)-hz.at(x-1,y,z))*params.cx - (hx.at(x,y,z)-hx.at(x,y,z-1))*params.cz);
			}
		}
	}
	return raft::proceed;
}

raft::kstatus Ez::run()
{
	if (iterations-- == 0) {
		output["Final"].push(grid);
		return raft::stop;
	}

	output["out_A"].push(grid);
	output["out_B"].push(grid);

	Grid hx, hy;
	input["A"].pop(hx);
	input["B"].pop(hy);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				grid.at(x, y, z) -= params.ce * ((hx.at(x,y,z)-hx.at(x,y-1,z))*params.cy - (hy.at(x,y,z)-hy.at(x-1,y,z))*params.cx);
			}
		}
	}
	return raft::proceed;
}
