${VULKAN_SDK}/bin/glslc shader.vert -o spirv/shader-vert.spv
${VULKAN_SDK}/bin/glslc shader.frag -o spirv/shader-frag.spv
${VULKAN_SDK}/bin/glslc cull.comp -o spirv/cull-comp.spv
${VULKAN_SDK}/bin/glslc compact.comp -o spirv/compact-comp.spv