# ------------------------------------------------------------------------
#
# check deformation
#

set(TEST_BASE_DIR ${PROJECT_SOURCE_DIR}/tests/gama-local-deformation)
set(GAMA_LOCAL_DEFORMATION ${CMAKE_BINARY_DIR}/gama-local-deformation)
set(RESULT_TOP tests/gama-local-deformation/results${PROJECT_VERSION})

#
# ------------------------------------------------------------------------
#
# check deformation - diff section
#

set(INPUT_DIR ${TEST_BASE_DIR}/input/diff)
set(RESULT_DIR ${CMAKE_BINARY_DIR}/${RESULT_TOP}/diff)
file(MAKE_DIRECTORY ${RESULT_DIR})

add_test(NAME check_deformation_data_1_2_delete COMMAND rm -f
  ${RESULT_DIR}/data-1-2.txt ${RESULT_DIR}/data-1-2.svg
)

add_test(NAME check_deformation_data_1_2_generate
  COMMAND ${GAMA_LOCAL_DEFORMATION}
  ${INPUT_DIR}/data-1.xml ${INPUT_DIR}/data-2.xml
  --text ${RESULT_DIR}/data-1-2.txt --svg ${RESULT_DIR}/data-1-2.svg
)

add_test(NAME check_deformation_data_1_2_diff_txt COMMAND diff -s
  ${INPUT_DIR}/data-1-2.txt ${RESULT_DIR}/data-1-2.txt
)

add_test(NAME check_deformation_data_1_2_diff_svg COMMAND diff -s
  ${INPUT_DIR}/data-1-2.svg ${RESULT_DIR}/data-1-2.svg
)
