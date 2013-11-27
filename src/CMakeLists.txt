add_library(dmp STATIC dmp_pool.c dmp.c)
install(TARGETS dmp DESTINATION lib)
