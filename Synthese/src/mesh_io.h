
#ifndef _MESH_IO_H
#define _MESH_IO_H

#include <vector>

#include "vec.h"


bool read_positions( const char *filename, std::vector<Point>& positions );
bool read_indexed_positions( const char *filename, std::vector<Point>& positions, std::vector<int>& indices );

#endif
