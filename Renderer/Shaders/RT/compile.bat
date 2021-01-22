%VULKAN_SDK%\Bin\glslc.exe --target-spv=spv1.4 --target-env=vulkan1.2 raytrace.rchit -o rchit.spv
%VULKAN_SDK%\Bin\glslc.exe --target-spv=spv1.4 --target-env=vulkan1.2 raytrace.rgen -o rgen.spv
%VULKAN_SDK%\Bin\glslc.exe --target-spv=spv1.4 --target-env=vulkan1.2 raytrace.rmiss -o rmiss.spv
pause