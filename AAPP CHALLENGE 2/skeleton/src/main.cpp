#include "data.hpp"
#include "process.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <sstream>

extern "C" {
#include "mpi_error_check.h"
}

int main(int argc, char* argv[]) {
  // initialize the MPI environment
  int provided_thread_level;
  const int rc_init = MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided_thread_level);
  exit_on_fail(rc_init);
  if (provided_thread_level < MPI_THREAD_SINGLE) {
    std::cerr << "Minimum MPI level not satisfied ..." << std::endl;
    std::cerr << "Out of curiosity, which implementation are you using?" << std::endl;
    return EXIT_FAILURE;
  }

  //==---------------------------------------------------------------------------------------------------==//
  // DATA GENERATION PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // This phase aim at generating the dataset to compute. It mimicks the operation of reading molecules from
  // a file and parse them. Leave this section as it is, at the end the process that happen to have rank 0
  // will have all the initialized data.
  // Please, note that the number of molecules comes from a command line argument, so it is known at runtime
  // but YOU CAN ASSUME THAT THEY ARE A MULTIPLE OF THE NUMBER PROCESSSES

  int world_rank    = 0;
  const int rc_rank = MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  exit_on_fail(rc_rank);
  auto data = std::vector<chem::molecule>{};
  if (world_rank == 0) {
    if (argc < 2) {
      std::cerr << "Error: illegal parameter number!" << std::endl;
      std::cout << "Usage:" << std::endl;
      std::cout << "\t" << argv[0] << " num_data" << std::endl;
      return EXIT_FAILURE;
    }
    auto parser   = std::istringstream(argv[1]);
    auto num_data = std::uint32_t{0};
    parser >> num_data;
    if (parser.fail()) {
      std::cerr << "Error: unable to understand the number \"" << argv[1] << '"' << std::endl;
      return EXIT_FAILURE;
    }
    data = chem::generate_data(num_data);
  }

  //==---------------------------------------------------------------------------------------------------==//
  // COMPUTATION PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // You should distribute the computation across all the processes that the user launched. In this skeleton
  // application the rank 0 has all the data and computes all the data. You can use this code to generate the
  // reference data and check if they remain the same while changing the number of processes

  for (auto& molecule: data) { chem::score(molecule); }

  //==---------------------------------------------------------------------------------------------------==//
  // REDUCTION PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // Each process should have the scores of an input subset. The goal of this phase is to perform a reduction
  // and collect in one process the global optimal 1% of molecules. In this skeleton application is trivial
  // since the process with rank 0 has already all the data.

  const auto num_output =
      std::max(static_cast<std::size_t>(static_cast<float>(data.size()) * 0.01), std::size_t{1});
  std::partial_sort(std::begin(data),
                    std::begin(data) + num_output,
                    std::end(data)); // lower indexes better molecules
  if (data.size() > num_output)
    data.resize(num_output);

  //==---------------------------------------------------------------------------------------------------==//
  // OUTPUT PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // The process with the output must call this function to display the data. The validator will use those
  // lines to check whether the application provides the correct answer.
  print_data(data);

  // clear the MPI environment
  const int rc_finalize = MPI_Finalize();
  exit_on_fail(rc_finalize);

  // if we reach this point, everything is fine
  return EXIT_SUCCESS;
}
