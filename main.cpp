#include "fdtd.hpp"
#include "grid.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>

[[noreturn]]
static void usage(const char *progname)
{
	std::cerr << "usage: " << progname << " x y z t" << std::endl;
	std::cerr << "  x: Number of grid units in the X direction (>0)" << std::endl;
	std::cerr << "  y: Number of grid units in the Y direction (>0)" << std::endl;
	std::cerr << "  z: Number of grid units in the Z direction (>0)" << std::endl;
	std::cerr << "  t: Number of time steps (iterations) to simulate (>=0), 0 = infinite" << std::endl;
	std::exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int dimX, dimY, dimZ;
	int t;

	if (argc < 5)
		usage(argv[0]);
	dimX = std::atoi(argv[1]);
	dimY = std::atoi(argv[2]);
	dimZ = std::atoi(argv[3]);
	t    = std::atoi(argv[4]);
	if (dimX <= 0 || dimY <= 0 || dimZ <= 0 || t < 0)
		usage(argv[0]);

	return fdtd(dimX, dimY, dimZ, t);
}
