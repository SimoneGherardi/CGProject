param($Name)
glslc -o "$Name.vert.spv" "$Name.vert"
glslc -o "$Name.frag.spv" "$Name.frag"