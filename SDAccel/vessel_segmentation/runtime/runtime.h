#pragma once

#include "xcl.h"
#include <stdio.h>

struct _caos_runtime_t {
	xcl_world world;
	cl_program program;
	cl_kernel match_filter_caos_kernel;
} _caos_runtime;

void _free_caos_runtime()
{
	clReleaseKernel(_caos_runtime.match_filter_caos_kernel);
    clReleaseProgram(_caos_runtime.program);
    xcl_release_world(_caos_runtime.world);
}

void caos_init_runtime(int *argc, char **argv[])
{
	char *tmp;

	if(*argc < 2) {
		printf("xclbin path is missing.\n");
		exit(-1);
	}
    _caos_runtime.world = xcl_world_single();
    _caos_runtime.program = xcl_import_binary_file(_caos_runtime.world, (*argv)[1]);
	_caos_runtime.match_filter_caos_kernel = xcl_get_kernel(_caos_runtime.program, "match_filter_caos_kernel");
    atexit(&_free_caos_runtime);

    // swap the first 2 arguments
    tmp = (*argv)[0];
    (*argv)[0] = (*argv)[1];
    (*argv)[1] = tmp;

    // remove the first argument (i.e. the path to the xclbin file)
    *argc = *argc - 1;
    *argv = *argv + 1;
}