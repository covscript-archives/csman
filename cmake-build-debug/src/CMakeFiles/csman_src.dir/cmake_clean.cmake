file(REMOVE_RECURSE
  "libcsman_src.a"
  "libcsman_src.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/csman_src.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
