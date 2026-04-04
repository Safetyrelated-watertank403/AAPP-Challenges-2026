/**
 * Author : Andrea Bellani (C.P. 10733192)
 * 
 * Last Modified: 13th december 2025
**/

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

#define BEST_FRACTION 0.01 //fraction of molecules that are considered the best

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
  
  int world_size;
  const auto rc_size = MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  exit_on_fail(rc_size);

  size_t elements_per_proc;

  if (world_rank == 0)
    {
      elements_per_proc = data.size()/world_size; //I distribute in equal measure molecules to processes
    }

  /**
   * Since other processes do not know in advance "data.size()" (provided that we do not change the "DATA GENERATION PHASE")
   * I first broadcast it from P0 (other possibilities: not using "Scatter" to distribute but "Recv" with "Probe")
  **/
  const int rc_bcast = MPI_Bcast(&elements_per_proc, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  exit_on_fail(rc_bcast);
  
  std::vector<chem::molecule> my_molecules(elements_per_proc); //where each process is going to store its top molecules
  
  MPI_Datatype MPI_MOL = chem::create_molecule_MPI_type(); //I define it one time here

  const int rc_scatter = MPI_Scatter(
    data.data(),
    elements_per_proc,
    MPI_MOL,
    my_molecules.data(),
    elements_per_proc,
    MPI_MOL,
    0,
    MPI_COMM_WORLD
  );
  exit_on_fail(rc_scatter);

  //now molecules are distributed equally among processors
  for (auto& molecule: my_molecules) { chem::score(molecule); }

  //==---------------------------------------------------------------------------------------------------==//
  // REDUCTION PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // Each process should have the scores of an input subset. The goal of this phase is to perform a reduction
  // and collect in one process the global optimal BEST_FRACTION of molecules. In this skeleton application is trivial
  // since the process with rank 0 has already all the data.

  /**
   * In principle, a process should send n*BEST_FRACTION elements (where "n" is molecules number), since it has a priori
   * no infos regardining other processes molecules (i.e. its n*BEST_FRACTION best molecules may be also the
   * n*BEST_FRACTION best overall).
   * 
   * A corner case is the one in which each process has less molecules than "n*BEST_FRACTION"
   * (e.g. n=200 and world_size=200, with BEST_FRACTION=0.01).
   * In this case the process sorts and sends them all.
  **/

  const auto elements_to_send = std::min(
    elements_per_proc,
    std::max(
      static_cast<std::size_t>(static_cast<float>(elements_per_proc*world_size) * BEST_FRACTION),
      std::size_t{1}
  ));

  //this is the sorting logic Professor Gadioli shared in the template
  std::partial_sort(std::begin(my_molecules),
                    std::begin(my_molecules) + elements_to_send,
                    std::end(my_molecules));
  if (my_molecules.size() > elements_to_send)
    my_molecules.resize(elements_to_send);

  /**
   * At this point, each process has selected its top "n*BEST_FRACTION" molecules properly sorted. From now on, no more sorting
   * is needed, we can just merge each "partial top molecules vector" by performing what is basically a reduction
   * (complexity is logarithmic). This approach was inspired by the "logarithmic sum" example (as well as the 
   * "Reduction Pattern" explained by Professor Ferrandi).
   * 
   * The reduction is just a "merge + limit", where the "limit" is on "n*BEST_FRACTION".
  **/

  uint32_t i = 2;

  uint32_t P = 1;

  //we compute the number of "rounds" of the reduction (i.e. "log_2(world_size)+1")
  /**
   * The reason why we perform "log_2(world_size)+1" rounds instead of "log_2(world_size)" is the following:
   * 
   * in order to ensuring that even with "world_size" not a power of two the algorithm works as expected,
   * we have to change a bit the classic "logarithmic reduction" : if a process have gathered some molecules
   * that still need to be merged but there is no process to merge them with (e.g. if world_size=10: P8 first merges
   * with P9 but in the last round it has to merge with P0, therefore we have to perform at most "log_2(world_size)+1")
  **/
  while (P<= world_size)
    P <<= 1;

  while (i <= P)
    {
      /**
       * To better understand the logic: let's define two different roles in each pair of processes that communicate each other in the same round:
       * -> the "merger": the process that is going to merge elements from itself and from the "colleague";
       * -> the "colleague": the process that just sends its top-molecules to its "merger".
      **/
      bool should_i_participate =
        world_rank%i == 0 ? //"am I a merger?"
          (world_rank+(i/2))<world_size : //"I participate as a merger only if my colleague exists" (it may not in case "world_size" is not power of two)
          world_rank%i == i/2 //"if I'm not a merger I paricipate only if I am a colleague" (If I am a colleague, my merger always exists)
      ;

      if (should_i_participate)
        {
          if (world_rank%i == 0) //"am I a merger?"
            {
              MPI_Status status;
              int elements_to_receive;

              //merger needs to probe since there is the possibility that the colleague sends less then "n*BEST_FRACTION" molecules in case it has not enough molecules
              const int rc_probe = MPI_Probe(world_rank+(i/2), 0, MPI_COMM_WORLD, &status);
              exit_on_fail(rc_probe);
              const int rc_get_count = MPI_Get_count(&status, MPI_MOL, &elements_to_receive);
              exit_on_fail(rc_get_count);

              //receive from your colleague
              std::vector<chem::molecule> received_molecules(elements_to_receive);
              const int rc_recv = MPI_Recv(received_molecules.data(), elements_to_receive, MPI_MOL, world_rank+(i/2), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
              exit_on_fail(rc_recv);

              //merge
              std::vector<chem::molecule> app(my_molecules.size() + received_molecules.size());
              std::merge(my_molecules.begin(), my_molecules.end(), received_molecules.begin(), received_molecules.end(), app.begin());

              //limit your top-molecules
              app.resize(
                std::min(
                  (size_t) my_molecules.size()+elements_to_receive,
                  std::max(
                    static_cast<std::size_t>(static_cast<float>(elements_per_proc*world_size) * BEST_FRACTION),
                    std::size_t{1}))
              );
              my_molecules = std::move(app);

            }
          else //"I am a colleague"
            {
              //sends your top-molecules to your merger
              const int rc_ssend = MPI_Ssend(my_molecules.data(), my_molecules.size(), MPI_MOL, world_rank-(i/2), 0, MPI_COMM_WORLD);
              exit_on_fail(rc_ssend);
            }
        }
        
      i *= 2;
    }

  //Note that at this point process 0 has just the full set of top 1% molecules inside "my_molecules", so the computation is finished

  /**
   * ------------------------------------------------------------------------------------------------------------------------------------
   *                                                            COMPLEXITY ANALYSIS
   * ------------------------------------------------------------------------------------------------------------------------------------
   * 
   * If "P" is the number of processes and "N" is the overall amount of molecules (assume "P<N*BEST_FRACTION" for simplicity)
   * the elapsed time of our algorithm is:
   * -> sorting_cost(N/P) + log_2(P)*merging_cost(2 * (N*BEST_FRACTION)) [assume for simplicity that "P" is a power of two]
   * 
   * IF we select a polyLog sorting algorithm and a linear merging algorithm:
   * -> T_P = (N/P)*log(N/P) + 2*log(P)*N*BEST_FRACTION
   * 
   * The best sequential algorithm has a complexity of:
   * -> T* = sorting_cost(N) -> N*log(N)
   * 
   * therefore the solution has a speed-up of:
   * -> SU_P = N*log(N)/[(N/P)*log(N/P) + 2*log(P)*N*BEST_FRACTION] =
   *         = log(N)/[log(N/P)/P + 2*log(P)*BEST_FRACTION] =
   *         = P*log(N)/[log(N/P) + 2*P*log(P)*BEST_FRACTION] -> N>>P -> P
   * 
   * In addition, we see that our parallel solution exploits "strong scaling" (just assume "P<N*BEST_FRACTION"): we can arbitrarly increasing "P"
   * without changing the "N" to linearly increase the speed-up.
  **/

  //==---------------------------------------------------------------------------------------------------==//
  // OUTPUT PHASE
  //==---------------------------------------------------------------------------------------------------==//
  // The process with the output must call this function to display the data. The validator will use those
  // lines to check whether the application provides the correct answer.
   
  if (world_rank == 0)
    print_data(my_molecules);

  // clear the MPI environment
  const int rc_finalize = MPI_Finalize();
  exit_on_fail(rc_finalize);

  // if we reach this point, everything is fine
  return EXIT_SUCCESS;
}