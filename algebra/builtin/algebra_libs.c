#include "osqp_api_constants.h"
#include "osqp_api_types.h"
#include "qdldl_interface.h"
#include "profilers.h"
#include "util.h"

OSQPInt osqp_algebra_linsys_supported(void) {
  /* Only has QDLDL (direct solver) */
  return OSQP_CAPABILITY_DIRECT_SOLVER;
}

enum osqp_linsys_solver_type osqp_algebra_default_linsys(void) {
  /* Prefer QDLDL (it is also the only one available) */
  return OSQP_DIRECT_SOLVER;
}

OSQPInt osqp_algebra_init_libs(OSQPInt device)
{
  OSQP_UnusedVar(device);
  return 0;
}

void osqp_algebra_free_libs(void) {return;}

OSQPInt osqp_algebra_name(char* name, OSQPInt nameLen) {
  OSQP_UnusedVar(nameLen);

  // Manually assign into the buffer to avoid using strcpy
  name[0] = 'B';
  name[1] = 'u';
  name[2] = 'i';
  name[3] = 'l';
  name[4] = 't';
  name[5] = '-';
  name[6] = 'i';
  name[7] = 'n';
  name[8] = 0;

  return 9;
}

OSQPInt osqp_algebra_device_name(char* name, OSQPInt nameLen) {
  OSQP_UnusedVar(nameLen);

  /* No device name for built-in algebra */
  name[0] = 0;
  return 0;
}

#ifndef OSQP_EMBEDDED_MODE

// Initialize linear system solver structure
// NB: Only the upper triangular part of P is filled
OSQPInt osqp_algebra_init_linsys_solver(LinSysSolver**      s,
                                        const OSQPMatrix*   P,
                                        const OSQPMatrix*   A,
                                        const OSQPVectorf*  rho_vec,
                                        const OSQPSettings* settings,
                                        OSQPFloat*          scaled_prim_res,
                                        OSQPFloat*          scaled_dual_res,
                                        OSQPInt             polishing) {
  OSQPInt retval = 0;

  // Don't use the scaled residuals right now
  OSQP_UnusedVar(scaled_prim_res);
  OSQP_UnusedVar(scaled_dual_res);

  osqp_profiler_sec_push(OSQP_PROFILER_SEC_LINSYS_INIT);

  switch (settings->linsys_solver) {
  default:
  case OSQP_DIRECT_SOLVER:
    retval = init_linsys_solver_qdldl((qdldl_solver **)s, P, A, rho_vec, settings, polishing);
  }

  osqp_profiler_sec_pop(OSQP_PROFILER_SEC_LINSYS_INIT);
  return retval;
}

OSQPInt adjoint_derivative_linsys_solver(LinSysSolver**      s,
                                         const OSQPSettings* settings,
                                         const OSQPMatrix*   P,
                                         const OSQPMatrix*   G,
                                         const OSQPMatrix*   A_eq,
                                         OSQPMatrix*         GDiagLambda,
                                         OSQPVectorf*        slacks,
                                         OSQPVectorf*        rhs) {

  OSQP_UnusedVar(settings);

  return adjoint_derivative_qdldl((qdldl_solver **)s, P, G, A_eq, GDiagLambda, slacks, rhs);
}

#endif
