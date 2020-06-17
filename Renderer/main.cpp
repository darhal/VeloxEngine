#include "pch.h"

#if defined(BUILD_LIB)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#elif defined(BUILD_EXEC)

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

int main()
{

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	std::vector<const char*> extensionsNames(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	int i = 0;
	std::cout << "available extensions:\n";
	for (const auto& extension : extensions) {
		std::cout << '\t' << extension.extensionName << '\n';
		extensionsNames[i++] = extension.extensionName;
	}

	getchar();
}

#endif