#pragma once

#define TRE_LOGI(...) \
	{ fprintf(stdout, "[TRE][RENDERER] (INFO): "); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }\

#define TRE_LOGD(...) \
	{ fprintf(stdout, "[TRE][RENDERER] (DEBUG): "); fprintf(stdout,__VA_ARGS__); fprintf(stdout, "\n"); }\

#define TRE_LOGE(...) \
	{ fprintf(stderr, "[TRE][RENDERER] (ERROR): "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }\

#define TRE_LOGW(...) \
	{ fprintf(stderr, "[TRE][RENDERER] (WARNING): "); fprintf(stderr,__VA_ARGS__); fprintf(stderr, "\n"); }\

