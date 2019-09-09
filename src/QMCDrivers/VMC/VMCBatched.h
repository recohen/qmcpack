//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2019 developers.
//
// File developed by: Peter Doak, doakpw@ornl.gov, Oak Ridge National Laboratory
//
// File refactored from VMC.h
//////////////////////////////////////////////////////////////////////////////////////


#ifndef QMCPLUSPLUS_VMCBATCHED_H
#define QMCPLUSPLUS_VMCBATCHED_H

#include "QMCDrivers/QMCDriverNew.h"
#include "QMCDrivers/VMC/VMCDriverInput.h"
#include "QMCDrivers/MCPopulation.h"
#include "QMCDrivers/ContextForSteps.h"
#include "QMCDrivers/GreenFunctionModifiers/DriftModifierBase.h"

namespace qmcplusplus
{
/** @ingroup QMCDrivers  ParticleByParticle
 * @brief Implements a VMC using particle-by-particle move. Threaded execution.
 */
class VMCBatched : public QMCDriverNew
{
public:
  using PosType = QMCTraits::PosType;
  using ParticlePositions = PtclOnLatticeTraits::ParticlePos_t;

  /** To avoid 10's of arguments to runVMCStep
   *
   *  There should be a division between const input to runVMCStep
   *  And step to step state
   */
  struct StateForThread
  {
    QMCDriverInput& qmcdrv_input;
    VMCDriverInput& vmcdrv_input;
    MCPopulation&   population;
    IndexType recalculate_properties_period;
    IndexType step;
    int block;
    bool recomputing_blocks;
    DriftModifierBase& drift_modifier;
    StateForThread(QMCDriverInput&  qmci,
                   VMCDriverInput& vmci,
                   DriftModifierBase& drift_mod,
                   MCPopulation&   pop) : qmcdrv_input(qmci), vmcdrv_input(vmci), drift_modifier(drift_mod), population(pop) {}

                   
  };

public:
  /// Constructor.
  VMCBatched(QMCDriverInput&& qmcdriver_input,
             VMCDriverInput&& input,
             MCPopulation&& pop,
             TrialWaveFunction& psi,
             QMCHamiltonian& h,
             WaveFunctionPool& ppool,
             Communicate* comm);
 
  bool run();

  /** Refactor of VMCUpdatePbyP in crowd context
   *
   *  MCWalkerConfiguration layer removed.
   *  Obfuscation of state changes via buffer and MCWalkerconfiguration require this be tested well
   */
  static void advanceWalkers(const StateForThread& sft, Crowd& crowd, ContextForSteps& move_context, bool recompute);
  
  // This is the task body executed at crowd scope
  // it does not have access to object member variables by design
  static void runVMCStep(int crowd_id,
                         const StateForThread& sft,
                         std::vector<std::unique_ptr<ContextForSteps>>& move_context,
                         std::vector<std::unique_ptr<Crowd>>& crowds);
  void setup();
  //inline std::vector<RandomGenerator_t*>& getRng() { return Rng;}
  IndexType calc_default_local_walkers();

private:
  int prevSteps;
  int prevStepsBetweenSamples;
  VMCDriverInput vmcdriver_input_;
  QMCRunType getRunType() { return QMCRunType::VMC_BATCH; }
  ///Ways to set rn constant
  RealType logoffset, logepsilon;
  ///option to enable/disable drift equation or RN for VMC
  std::string UseDrift;
  ///check the run-time environments
  void resetRun();
  ///copy constructor
  VMCBatched(const VMCBatched&) = delete;
  /// Copy operator (disabled).
  VMCBatched& operator=(const VMCBatched&) = delete;
};

extern std::ostream& operator<<(std::ostream& o_stream, const VMCBatched& vmc_batched);
} // namespace qmcplusplus

#endif
