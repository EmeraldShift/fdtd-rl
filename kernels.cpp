#include "kernels.hpp"

#include <cstdlib>
#include <iostream>

InitialGridGenerator::InitialGridGenerator(dim_t x, dim_t y, dim_t z, unsigned ports) : x(x), y(y), z(z), ports(ports)
{
	for (unsigned i = 0; i < ports; i++)
		output.addPort<Grid>(std::to_string(i));
}

raft::kstatus InitialGridGenerator::run()
{
	Grid g(x, y, z);
	for (dim_t i = 0; i < x * y * z; i++)
		g[i] = drand48();
	for (auto &p : output)
		p.push(g);
	return raft::stop;
}

Loopback::Loopback()
{
	input.addPort<Grid>("initial");
	input.addPort<Grid>("loop");
	output.addPort<Grid>("out");
}

raft::kstatus Loopback::run() {
	std::string label = "loop";
	if (initial) {
		initial = false;
		label = "initial";
	}
	Grid g;
	input[label].pop<Grid>(g);
	output["out"].push(g);
	return raft::proceed;
}

GridPrinter::GridPrinter()
{
	input.addPort<Grid>("grid");
}

raft::kstatus GridPrinter::run()
{
	Grid g;
	input["grid"].pop(g);
	std::stringstream ss;
	ss << "Grid(" << g.x() << ", " << g.y() << ", " << g.z() << ") = [ ";
	for (dim_t i = 0; i < g.x() * g.y() * g.z(); i++)
		ss << g[i] << " ";
	ss << "]" << std::endl;
	std::cout << ss.rdbuf();
	return raft::proceed;
}

Hx::Hx(phys::params params, unsigned long i) : raft::kernel(), params(params), iterations(i)
{
	input.addPort<Grid>("init_Hx");
	input.addPort<Grid>("init_Ey");
	input.addPort<Grid>("init_Ez");
	input.addPort<Grid>("Hx");
	input.addPort<Grid>("Ey");
	input.addPort<Grid>("Ez");
	output.addPort<Grid>("out_Hx");
	output.addPort<Grid>("out_Ey");
	output.addPort<Grid>("out_Ez");
	output.addPort<Grid>("Final");
}

raft::kstatus Hx::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Hx"].peek<Grid>());
		return raft::stop;
	}

	Grid hx, ey, ez;
	if (initial) {
		initial = false;
		input["init_Hx"].pop(hx);
		input["init_Ey"].pop(ey);
		input["init_Ez"].pop(ez);
	} else {
		input["Hx"].pop(hx);
		input["Ey"].pop(ey);
		input["Ez"].pop(ez);
	}

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				hx.at(x, y, z) += params.ch * ((ey.at(x,y,z+1)-ey.at(x,y,z))*params.cz - (ez.at(x,y+1,z)-ez.at(x,y,z))*params.cy);
			}
		}
	}

	output["out_Hx"].push(hx);
	output["out_Ey"].push(hx);
	output["out_Ez"].push(hx);
	return raft::proceed;
}

Hy::Hy(phys::params params, unsigned long i) : params(params), iterations(i)
{
	input.addPort<Grid>("init_Hy");
	input.addPort<Grid>("init_Ez");
	input.addPort<Grid>("init_Ex");
	input.addPort<Grid>("Hy");
	input.addPort<Grid>("Ez");
	input.addPort<Grid>("Ex");
	output.addPort<Grid>("out_Hy");
	output.addPort<Grid>("out_Ez");
	output.addPort<Grid>("out_Ex");
	output.addPort<Grid>("Final");
}

raft::kstatus Hy::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Hy"].peek<Grid>());
		return raft::stop;
	}

	Grid hy, ez, ex;
	if (initial) {
		initial = false;
		input["init_Hy"].pop(hy);
		input["init_Ez"].pop(ez);
		input["init_Ex"].pop(ex);
	} else {
		input["Hy"].pop(hy);
		input["Ez"].pop(ez);
		input["Ex"].pop(ex);
	}

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				hy.at(x, y, z) += params.ch * ((ez.at(x+1,y,z)-ez.at(x,y,z))*params.cx - (ex.at(x,y,z+1)-ex.at(x,y,z))*params.cz);
			}
		}
	}

	output["out_Hy"].push(hy);
	output["out_Ez"].push(hy);
	output["out_Ex"].push(hy);
	return raft::proceed;
}

Hz::Hz(phys::params params, unsigned long i) : params(params), iterations(i)
{
	input.addPort<Grid>("init_Hz");
	input.addPort<Grid>("init_Ex");
	input.addPort<Grid>("init_Ey");
	input.addPort<Grid>("Hz");
	input.addPort<Grid>("Ex");
	input.addPort<Grid>("Ey");
	output.addPort<Grid>("out_Hz");
	output.addPort<Grid>("out_Ex");
	output.addPort<Grid>("out_Ey");
	output.addPort<Grid>("Final");
}

raft::kstatus Hz::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Hz"].peek<Grid>());
		return raft::stop;
	}

	Grid hz, ex, ey;
	if (initial) {
		initial = false;
		input["init_Hz"].pop(hz);
		input["init_Ex"].pop(ex);
		input["init_Ey"].pop(ey);
	} else {
		input["Hz"].pop(hz);
		input["Ex"].pop(ex);
		input["Ey"].pop(ey);
	}

	for (dim_t x = 0; x < params.nx-1; x++) {
		for (dim_t y = 0; y < params.ny-1; y++) {
			for (dim_t z = 0; z < params.nz-1; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				hz.at(x, y, z) += params.ch * ((ex.at(x,y+1,z)-ex.at(x,y,z))*params.cy - (ey.at(x+1,y,z)-ey.at(x,y,z))*params.cx);
			}
		}
	}

	output["out_Hz"].push(hz);
	output["out_Ex"].push(hz);
	output["out_Ey"].push(hz);
	return raft::proceed;
}

Ex::Ex(phys::params params, unsigned long i) : params(params), iterations(i)
{
	input.addPort<Grid>("init_Ex");
	input.addPort<Grid>("Ex");
	input.addPort<Grid>("Hy");
	input.addPort<Grid>("Hz");
	output.addPort<Grid>("out_Ex");
	output.addPort<Grid>("out_Hy");
	output.addPort<Grid>("out_Hz");
	output.addPort<Grid>("Final");
}

raft::kstatus Ex::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Ex"].peek<Grid>());
		return raft::stop;
	}

	Grid ex, hy, hz;
	if (initial) {
		initial = false;
		input["init_Ex"].pop(ex);
	} else {
		input["Ex"].pop(ex);
	}
	input["Hy"].pop(hy);
	input["Hz"].pop(hz);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				ex.at(x, y, z) -= params.ce * ((hy.at(x,y,z)-hy.at(x,y,z-1))*params.cz - (hz.at(x,y,z)-hz.at(x,y-1,z))*params.cy);
			}
		}
	}

	output["out_Ex"].push(ex);
	output["out_Hy"].push(ex);
	output["out_Hz"].push(ex);
	return raft::proceed;
}

Ey::Ey(phys::params params, unsigned long i) : params(params), iterations(i)
{
	input.addPort<Grid>("init_Ey");
	input.addPort<Grid>("Ey");
	input.addPort<Grid>("Hz");
	input.addPort<Grid>("Hx");
	output.addPort<Grid>("out_Ey");
	output.addPort<Grid>("out_Hz");
	output.addPort<Grid>("out_Hx");
	output.addPort<Grid>("Final");
}

raft::kstatus Ey::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Ey"].peek<Grid>());
		return raft::stop;
	}

	Grid ey, hz, hx;
	if (initial) {
		initial = false;
		input["init_Ey"].pop(ey);
	} else {
		input["Ey"].pop(ey);
	}
	input["Hz"].pop(hz);
	input["Hx"].pop(hx);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				ey.at(x, y, z) -= params.ce * ((hz.at(x,y,z)-hz.at(x-1,y,z))*params.cx - (hx.at(x,y,z)-hx.at(x,y,z-1))*params.cz);
			}
		}
	}

	output["out_Ey"].push(ey);
	output["out_Hz"].push(ey);
	output["out_Hx"].push(ey);
	return raft::proceed;
}

Ez::Ez(phys::params params, unsigned long i) : params(params), iterations(i)
{
	input.addPort<Grid>("init_Ez");
	input.addPort<Grid>("Ez");
	input.addPort<Grid>("Hx");
	input.addPort<Grid>("Hy");
	output.addPort<Grid>("out_Ez");
	output.addPort<Grid>("out_Hx");
	output.addPort<Grid>("out_Hy");
	output.addPort<Grid>("Final");
}

raft::kstatus Ez::run()
{
	if (iterations-- == 0) {
		output["Final"].push(input["Ez"].peek<Grid>());
		return raft::stop;
	}

	Grid ez, hx, hy;
	if (initial) {
		initial = false;
		input["init_Ez"].pop(ez);
	} else {
		input["Ez"].pop(ez);
	}
	input["Hx"].pop(hx);
	input["Hy"].pop(hy);

	for (dim_t x = 1; x < params.nx; x++) {
		for (dim_t y = 1; y < params.ny; y++) {
			for (dim_t z = 1; z < params.nz; z++) {
				// FIXME This is done in place, but to stress-test libvl we might
				// want to replace this with a new copy that we push on the wire
				ez.at(x, y, z) -= params.ce * ((hx.at(x,y,z)-hx.at(x,y-1,z))*params.cy - (hy.at(x,y,z)-hy.at(x-1,y,z))*params.cx);
			}
		}
	}

	output["out_Ez"].push(ez);
	output["out_Hx"].push(ez);
	output["out_Hy"].push(ez);
	return raft::proceed;
}

