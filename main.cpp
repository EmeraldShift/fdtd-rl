#include "grid.hpp"
#include "fdtd.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>

[[noreturn]]
static void usage(const char *progname)
{
	std::cerr << "usage: " << progname << " x y z t [flags]" << std::endl;
	std::cerr << "  x: Number of grid units in the X direction (>0)" << std::endl;
	std::cerr << "  y: Number of grid units in the Y direction (>0)" << std::endl;
	std::cerr << "  z: Number of grid units in the Z direction (>0)" << std::endl;
	std::cerr << "  t: Number of time steps (iterations) to simulate (>=0), 0 = infinite" << std::endl;
	std::cerr << "supported flags:" << std::endl;
	std::cerr << "  d: Use dynamic queue resizing" << std::endl;
#ifdef QT
	std::cerr << "  q: Use QThread pool scheduling" << std::endl;
#endif // QT
#ifdef VL
	std::cerr << "  v: Use VirtualLink queues" << std::endl;
#endif // VL
	std::exit(EXIT_FAILURE);
}

static bool find_arg(char **argv, const char *arg)
{
	for (char **v = argv; *v; v++)
		if ((*v)[0] == '-' && std::strstr(&(*v)[1], arg))
			return true;
	return false;
}

int main(int argc, char **argv)
{
	dim_t dimX, dimY, dimZ;
	unsigned long t;

	if (argc < 5)
		usage(argv[0]);
	dimX = std::strtoul(argv[1], nullptr, 0);
	dimY = std::strtoul(argv[2], nullptr, 0);
	dimZ = std::strtoul(argv[3], nullptr, 0);
	t    = std::strtoul(argv[4], nullptr, 0);
	if (dimX == 0 || dimY == 0 || dimZ == 0)
		usage(argv[0]);

	unsigned long flags = 0;
	if (find_arg(argv, "q"))
		flags |= FLAG_QTH;
	if (find_arg(argv, "d"))
		flags |= FLAG_DYN;
	if (find_arg(argv, "v"))
		flags |= FLAG_VTL;

	return fdtd(dimX, dimY, dimZ, t, flags);
}
