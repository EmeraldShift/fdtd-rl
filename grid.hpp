#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

using dim_t = std::uint64_t;
using elem_t = double;

// A 3-dimensional grid of elements
class Grid {
	// Provided dimensions
	dim_t dimX, dimY, dimZ;
	// Pre-computed medium stride
	dim_t mstride;
	// Internal array
	elem_t *grid;

public:
	// Required by RaftLib for some reason
	Grid() : Grid(0, 0, 0) {}

	Grid(dim_t dimX, dim_t dimY, dim_t dimZ) : dimX(dimX), dimY(dimY), dimZ(dimZ) {
		mstride = dimX * dimY;
		grid = new elem_t[dimX * dimY * dimZ]();
	}

	Grid(Grid &o) : Grid(o.dimX, o.dimY, o.dimZ) {
		std::memcpy(grid, o.grid, (dimX * dimY * dimZ) * sizeof(elem_t));
	}

	Grid(Grid &&o) : dimX(o.dimX), dimY(o.dimY), dimZ(o.dimZ) {
		grid = o.grid;
		o.grid = nullptr;
	}

	~Grid() {
		delete[] grid;
	}

	// Deep copy here!
	// TODO: This is used (only?) by RaftLib, so we could probably hijack
	// this to make it shallow instead, and do any deep copies ourself.
	Grid &operator=(Grid &o) {
		dimX = o.dimX;
		dimY = o.dimY;
		dimZ = o.dimZ;
		mstride = dimX * dimY;
		delete[] grid;
		grid = new elem_t[dimX * dimY * dimZ];
		std::memcpy(grid, o.grid, (dimX * dimY * dimZ) * sizeof(elem_t));
		return *this;
	}

	elem_t &at(dim_t x, dim_t y, dim_t z) {
		return grid[x + (y * dimX) + (z * mstride)];
	}

	elem_t &operator[](dim_t index) {
		return grid[index];
	}

	dim_t x() {
		return dimX;
	}

	dim_t y() {
		return dimY;
	}

	dim_t z() {
		return dimZ;
	}
};
