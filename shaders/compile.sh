${VULKAN_SDK}/bin/glslc shader.vert -o spirv/shader-vert.spv
${VULKAN_SDK}/bin/glslc shader.frag -o spirv/shader-frag.spv
${VULKAN_SDK}/bin/glslc cull.comp -o spirv/cull-comp.spv
${VULKAN_SDK}/bin/glslc compact-draws.comp -o spirv/compact-draws-comp.spv
${VULKAN_SDK}/bin/glslc compact-bindless.comp -o spirv/compact-bindless-comp.spv
${VULKAN_SDK}/bin/glslc reset.comp -o spirv/reset-comp.spv