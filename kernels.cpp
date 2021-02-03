#include "kernels.hpp"

#include "grid.hpp"

#include <iostream>
#include <sstream>

ZeroGridGenerator::ZeroGridGenerator(dim_t x, dim_t y, dim_t z, int n) : x(x), y(y), z(z), n(n)
{
	output.addPort<Grid>("output");
}

raft::kstatus ZeroGridGenerator::run() {
	output["output"].push(Grid(x, y, z));
	if (--n == 0)
		return raft::stop;
	else
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

Hx::Hx()
{
	input.addPort<Grid>("Hx");
	//input.addPort<Grid>("Ey");
	//input.addPort<Grid>("Ez");
	output.addPort<Grid>("Hx");
}

// TODO: Compute the H_x field
raft::kstatus Hx::run()
{
	Grid g;
	input["Hx"].pop(g);

	// TODO: Update the H_x field

	output["Hx"].push(g);

	return raft::proceed;
}
