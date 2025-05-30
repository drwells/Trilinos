
TRIBITS_INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(UNIT_TEST_DRIVER
  ${PANZER_UNIT_TEST_MAIN})

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tSTKInterface
  SOURCES tSTKInterface.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 1
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tLineMeshFactory
  SOURCES tLineMeshFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tSquareQuadMeshFactory
  SOURCES tSquareQuadMeshFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tSquareTriMeshFactory
  SOURCES tSquareTriMeshFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE(
  tCubeHexMeshFactory
  SOURCES tCubeHexMeshFactory.cpp ${UNIT_TEST_DRIVER}
  )
TRIBITS_ADD_TEST(
  tCubeHexMeshFactory
  NUM_MPI_PROCS 2
  CATEGORIES NIGHTLY
  )

TRIBITS_ADD_EXECUTABLE(
  tCubeTetMeshFactory
  SOURCES tCubeTetMeshFactory.cpp ${UNIT_TEST_DRIVER}
  )
TRIBITS_ADD_TEST(
  tCubeTetMeshFactory
  NUM_MPI_PROCS 2
  CATEGORIES NIGHTLY
  )

# Above test takes more than 3 minutes to run on pu241 and therefore
# should not be a *BASIC* test.  It will still run in CI and Nightly
# testing.

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tSingleBlockCubeHexMeshFactory
  SOURCES tSingleBlockCubeHexMeshFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 4
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tSTK_IO
  SOURCES tSTK_IO.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 1
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tExodusReaderFactory
  SOURCES tExodusReaderFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tExodusIossProperties
  SOURCES tExodusIossProperties.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tExodusRestartAndAppend
  SOURCES tExodusRestartAndAppend.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tExodusEdgeBlock
  SOURCES tExodusEdgeBlock.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tExodusFaceBlock
  SOURCES tExodusFaceBlock.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tQuad8ToQuad4Factory
  SOURCES tQuad8ToQuad4Factory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tQuadraticToLinearMeshFactory
  SOURCES tQuadraticToLinearMeshFactory.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 2
  COMM serial mpi
  )

#TRIBITS_ADD_EXECUTABLE_AND_TEST(
#  tSolutionReader
#  SOURCES tSolutionReader.cpp ${UNIT_TEST_DRIVER}
#  NUM_MPI_PROCS 2
#  COMM serial mpi
#  )

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  tGhosting
  SOURCES tGhosting.cpp ${UNIT_TEST_DRIVER}
  NUM_MPI_PROCS 4
  COMM mpi
  )

IF (PANZER_HAVE_STKSEARCH)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    tCoarseSearch
    SOURCES tCoarseSearch.cpp ${UNIT_TEST_DRIVER}
    NUM_MPI_PROCS 4 
    COMM mpi
    )
ENDIF()

TRIBITS_ADD_EXECUTABLE(
  SquareMeshGen
  SOURCES SquareMeshGen.cpp
  )

IF (PANZER_HAVE_EPETRA_STACK)
  TRIBITS_ADD_EXECUTABLE(
    ScatterSpeed_Epetra
    SOURCES   tScatterSpeed_Epetra.cpp
  )
ENDIF()
#TRIBITS_ADD_EXECUTABLE_AND_TEST(
#  tUniformRef
#  SOURCES tUniformRef.cpp ${UNIT_TEST_DRIVER}
#  NUM_MPI_PROCS 2
#  COMM serial mpi
#  )


# Panzer has no direct dependency on ThreadPool, however the search
# portion of the stk library will be disabled if ThreadPool is
# disabled.  This test will fail to compile without the missing stk
# files.  So for now we will check to see if ThreadPool is
# enabled. STK will eventually remove this dependency.
#
# 10/18 KDD STK's dependence on ThreadPool has been removed #3463, and
# ThreadPool is being removed now #3445.  Yet, without the ThreadPool
# guard, this test does not build.
#
#IF(STK_ENABLE_ThreadPool)
#  TRIBITS_ADD_EXECUTABLE_AND_TEST(
#    tPointLocationSearch
#    SOURCES tPointLocationSearch.cpp ${UNIT_TEST_DRIVER}
#    NUM_MPI_PROCS 2
#    )
#ENDIF()
#
# 10/18 KDD a more appropriate guard for the above test is
#IF (${PROJECT_NAME}_ENABLE_STKSearch AND ${PROJECT_NAME}_ENABLE_STKSearchUtil)
# This guard induces compilation, but the compilation fails for me.
# This test appears to have fallen into disrepair.  #3719

TRIBITS_COPY_FILES_TO_BINARY_DIR(stk_interface_copy
  SOURCE_FILES basic.gen basic.gen.2.0 basic.gen.2.1 basic3d.gen basic3d.gen.2.0 basic3d.gen.2.1 twoblock_cube.gen twoblock_cube.gen.2.0 twoblock_cube.gen.2.1 quad8.jou quad8.gen 2block_cylinders_30deg.g 2block_cylinders_30deg.stp
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/meshes"
  DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/meshes"
  EXEDEPS tExodusReaderFactory
  )

TRIBITS_COPY_FILES_TO_BINARY_DIR(stk_interface_copy_quadconverter
SOURCE_FILES quad8.gen tet10.gen tri6.gen hex20.gen twoblock_cube_multitopo.gen
SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/meshes"
DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/meshes"
EXEDEPS tQuadraticToLinearMeshFactory
)

IF(${PACKAGE_NAME}_ENABLE_Pamgen)

  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    tPamgenMeshFactory
    SOURCES tPamgenMeshFactory.cpp ${UNIT_TEST_DRIVER}
    NUM_MPI_PROCS 1
    COMM serial mpi
    )

  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    tSTKRebalancing
    SOURCES tSTKRebalancing.cpp ${UNIT_TEST_DRIVER}
    NUM_MPI_PROCS 4
    COMM mpi
    )

  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    pamgen_writer
    SOURCES pamgen_writer.cpp
    )

  TRIBITS_COPY_FILES_TO_BINARY_DIR(stk_interface_copy_pamgen
    SOURCE_FILES pamgen_test.gen periodic_cylinder.pam
    DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}"
    EXEDEPS tPamgenMeshFactory pamgen_writer
    )

ENDIF()
