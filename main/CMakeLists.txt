
set(IDF_PATH $ENV{IDF_PATH})

set(include_dirs ".")

set(src_dirs ".")

idf_component_register(SRC_DIRS "${src_dirs}"
                    INCLUDE_DIRS "${include_dirs}")

