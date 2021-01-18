#pragma once

#define TRE_LOGI(...) \
	{ printf("[TRE][RENDERER] (INFO): "); printf(__VA_ARGS__); printf("\n"); }\

#define TRE_LOGD(...) \
	{ printf("\033[93m[TRE][RENDERER] (DEBUG): "); printf(__VA_ARGS__); printf("\033[0m\n"); }\

#define TRE_LOGE(...) \
	{ printf("\033[91m[TRE][RENDERER] (ERROR): "); printf(__VA_ARGS__); printf("\033[0m\n"); }\

#define TRE_LOGW(...) \
	{ printf("\033[93m[TRE][RENDERER] (WARNING): "); printf(__VA_ARGS__); printf("\033[0m\n"); }\

