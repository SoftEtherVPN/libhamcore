add_library(TestModule)
target_sources(TestModule
  PRIVATE
    RandomFile.c
    Utils.c
)
target_include_directories(TestModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(TestModule PUBLIC libhamcore ZLIB::ZLIB)

add_executable(TestEncodeDecode)
target_sources(TestEncodeDecode PRIVATE TestEncodeDecode.c)
target_link_libraries(TestEncodeDecode PRIVATE TestModule)

function(EncodeDecodeTest mode num_files)
  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/EncodeDecodeTest-${mode}-${num_files})
  file(MAKE_DIRECTORY ${working_dir})

  add_test(
    NAME TestEncodeDecode-${mode}-${num_files}
    COMMAND TestEncodeDecode ${mode} ${num_files}
    WORKING_DIRECTORY ${working_dir}
  )
endfunction()

EncodeDecodeTest(SMALL 1)
EncodeDecodeTest(SMALL 100)
EncodeDecodeTest(SMALL 1000)
EncodeDecodeTest(MEDIUM 1)
EncodeDecodeTest(MEDIUM 100)
EncodeDecodeTest(MEDIUM 500)
EncodeDecodeTest(LARGE 1)
EncodeDecodeTest(LARGE 100)
EncodeDecodeTest(LARGE 250)
EncodeDecodeTest(MIXED 10)
EncodeDecodeTest(MIXED 250)
EncodeDecodeTest(MIXED 500)

add_executable(TestArchiveFormat)
target_sources(TestArchiveFormat PRIVATE TestArchiveFormat.c)
target_link_libraries(TestArchiveFormat PRIVATE TestModule)

function(ArchiveFormatTest mode num_files)
  set(working_dir ${CMAKE_CURRENT_BINARY_DIR}/TestArchiveFormat-${mode}-${num_files})
  file(MAKE_DIRECTORY ${working_dir})

  add_test(
    NAME TestArchiveFormat-${mode}-${num_files}
    COMMAND TestArchiveFormat ${mode} ${num_files}
    WORKING_DIRECTORY ${working_dir}
  )
endfunction()

ArchiveFormatTest(SMALL 1)
ArchiveFormatTest(SMALL 100)
ArchiveFormatTest(SMALL 1000)
ArchiveFormatTest(MEDIUM 1)
ArchiveFormatTest(MEDIUM 100)
ArchiveFormatTest(MEDIUM 500)
ArchiveFormatTest(LARGE 1)
ArchiveFormatTest(LARGE 100)
ArchiveFormatTest(LARGE 250)
ArchiveFormatTest(MIXED 10)
ArchiveFormatTest(MIXED 250)
ArchiveFormatTest(MIXED 500)
