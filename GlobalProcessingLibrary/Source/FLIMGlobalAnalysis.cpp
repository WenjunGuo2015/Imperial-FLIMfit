
#include "FitStatus.h"
#include "ModelADA.h" 
#include "FLIMGlobalAnalysis.h"
#include "FLIMGlobalFitController.h"

#include <assert.h>

FLIMGlobalFitController* controller[MAX_CONTROLLER_IDX];
int id_registered[MAX_CONTROLLER_IDX];

#ifdef _WINDOWS

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
//#include <vld.h>		//visual (memory) leak detector
#include <stdlib.h>
#include <crtdbg.h>
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
      for(int i=0; i<MAX_CONTROLLER_IDX; i++)
      {
         controller[i] = NULL;
         id_registered[i] = 0;
      }
      break;

	case DLL_THREAD_ATTACH:
      break;
 
	case DLL_THREAD_DETACH:
      break;

	case DLL_PROCESS_DETACH:
      FLIMGlobalClearFit(-1);
      break;
	}
    return TRUE;
}

#else

void __attribute__ ((constructor)) myinit() 
{
      for(int i=0; i<MAX_CONTROLLER_IDX; i++)
      {
         controller[i] = NULL;
         id_registered[i] = 0;
      }
}

void __attribute__ ((destructor)) myfini()
{
   FLIMGlobalClearFit(-1);
}

#endif

FITDLL_API int FLIMGlobalGetUniqueID()
{
   for(int i=0; i<MAX_CONTROLLER_IDX; i++)
   {
      if (id_registered[i] == 0)
      {
         id_registered[i] = 1;
         return i;
      }
   }

   return -1;
}

FITDLL_API void FLIMGlobalRelinquishID(int id)
{
   id_registered[id] = 0;
}



int ValidControllerIdx(int c_idx)
{
   if (c_idx >= MAX_CONTROLLER_IDX)
      return false;

   if (controller[c_idx] == NULL)
	   return false;

   return (controller[c_idx]->init);
}

int CheckControllerIdx(int c_idx)
{
   if (c_idx >= MAX_CONTROLLER_IDX)
      return ERR_COULD_NOT_START_FIT;

   if (controller[c_idx] != NULL)
   {
      if (controller[c_idx]->status->IsRunning())
      {
         return ERR_FIT_IN_PROGRESS;
      }
      else
      {
         delete controller[c_idx];
         controller[c_idx] = NULL;
      }
   }

   return SUCCESS;
}

FITDLL_API int FLIMGlobalFitMemMap(int c_idx, int n_group, int n_px, int n_regions[], int global_mode,
                             int data_type, char *data_file, int mask[],
                             int n_t, doublereal t[],
                             int n_irf, doublereal t_irf[], doublereal irf[], doublereal pulse_pileup,
                             int n_exp, int n_fix, 
                             doublereal tau_min[], doublereal tau_max[], 
                             int single_guess, doublereal tau_guess[],
                             int fit_beta, double fixed_beta[],
                             int fit_t0, doublereal t0_guess, 
                             int fit_offset, doublereal offset_guess, 
                             int fit_scatter, doublereal scatter_guess,
                             int fit_tvb, double tvb_guess, double tvb_profile[],
                             int n_fret, int n_fret_fix, int inc_Rinf, double R_guess[],
                             int pulsetrain_correction, doublereal t_rep,
                             int ref_reconvolution, doublereal ref_lifetime_guess, int algorithm,
                             doublereal tau[], doublereal I0[], doublereal beta[], doublereal R[], double gamma[],
                             doublereal t0[], doublereal offset[], doublereal scatter[], double tvb[], double ref_lifetime[],
                             int calculate_errs, doublereal tau_err[], doublereal beta_err[], doublereal R_err[],
                             doublereal offset_err[], doublereal scatter_err[], doublereal tvb_err[], doublereal ref_lifetime_err[],
                             doublereal chi2[], int ierr[],
                             int n_thread, int runAsync, int use_callback, int (*callback)())
{

   int error = CheckControllerIdx(c_idx);
   if (error) 
		return error;

   if (!use_callback)
      callback = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( n_group, n_px, n_regions, global_mode,
                                      mask, n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      single_guess, tau_guess,
                                      fit_beta, fixed_beta,
                                      0, NULL,
                                      0, 0, 0, NULL,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, algorithm,
                                      tau, I0, beta, R, gamma, t0, NULL, NULL,
                                      offset, scatter, tvb, ref_lifetime, 
                                      calculate_errs, tau_err, beta_err, R_err, NULL, offset_err, 
                                      scatter_err, tvb_err, ref_lifetime_err,
                                      chi2, ierr,
                                      n_thread, runAsync, callback );

   error = controller[c_idx]->GetErrorCode();
   
   if (error) 
		return error;

   error = controller[c_idx]->SetData(data_file, data_type);
   
   if (error)
      return error;

   controller[c_idx]->Init();

   error = controller[c_idx]->RunWorkers();
   return error;

}


FITDLL_API int FLIMGlobalPolarisationFitMemMap(int c_idx, int n_group, int n_px, int n_regions[], int global_mode,
                             int data_type, char* data_file, int mask[],
                             int n_t, double t[],
                             int n_irf, double t_irf[], double irf[], double pulse_pileup,
                             int n_exp, int n_fix, 
                             double tau_min[], double tau_max[], 
                             int single_guess, double tau_guess[],
                             int fit_beta, double fixed_beta[],
                             int use_magic_decay, double magic_decay[],
                             int n_theta, int n_theta_fix, int inc_rinf, double theta_guess[],
                             int fit_t0, double t0_guess,
                             int fit_offset, double offset_guess, 
                             int fit_scatter, double scatter_guess,
                             int fit_tvb, double tvb_guess, double tvb_profile[],
                             int pulsetrain_correction, double t_rep,
                             int ref_reconvolution, double ref_lifetime_guess, int algorithm,
                             double tau[], double I0[], double beta[], double theta[], double r[], 
                             double t0[], double offset[], double scatter[], double tvb[], double ref_lifetime[],
                             int calculate_errs, doublereal tau_err[], doublereal beta_err[], doublereal theta_err[],
                             doublereal offset_err[], doublereal scatter_err[], doublereal tvb_err[], doublereal ref_lifetime_err[],
                             double chi2[], int ierr[],
                             int n_thread, int runAsync, int use_callback, int (*callback)())
{

   int error = CheckControllerIdx(c_idx);
   if (error)
      return error;

   if (!use_callback)
      callback = NULL;

   int n_fret = 0;
   int n_fret_fix = 0;
   int inc_Rinf = 0;
   double* R_guess = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( n_group, n_px, n_regions, global_mode,
                                      mask, n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      single_guess, tau_guess,
                                      fit_beta, fixed_beta,
                                      use_magic_decay, magic_decay,
                                      n_theta, n_theta_fix, inc_rinf, theta_guess,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, algorithm,
                                      tau, I0, beta, NULL, NULL, theta, r,
                                      t0, offset, scatter, tvb, ref_lifetime, 
                                      calculate_errs, tau_err, beta_err, NULL, theta_err, offset_err, 
                                      scatter_err, tvb_err, ref_lifetime_err,
                                      chi2, ierr,
                                      n_thread, runAsync, callback );

   controller[c_idx]->SetPolarisationMode(MODE_POLARISATION);
   
   error = controller[c_idx]->SetData(data_file, data_type);
   if (error)
      return error;

   controller[c_idx]->Init();
   
   error = controller[c_idx]->GetErrorCode();
   if (error)
      return error;

   error = controller[c_idx]->RunWorkers();
   return error;

}


FITDLL_API int FLIMGlobalFit(int c_idx, int n_group, int n_px, int n_regions[], int global_mode,
                             int data_type, doublereal data[], int mask[],
                             int n_t, doublereal t[],
                             int n_irf, doublereal t_irf[], doublereal irf[], doublereal pulse_pileup,
                             int n_exp, int n_fix,  doublereal tau_min[], doublereal tau_max[], 
                             int single_guess, doublereal tau_guess[],
                             int fit_beta, double fixed_beta[],
                             int fit_t0, doublereal t0_guess, 
                             int fit_offset, doublereal offset_guess, 
                             int fit_scatter, doublereal scatter_guess,
                             int fit_tvb, double tvb_guess, double tvb_profile[],
                             int n_fret, int n_fret_fix, int inc_Rinf, double R_guess[],
                             int pulsetrain_correction, doublereal t_rep,
                             int ref_reconvolution, doublereal ref_lifetime_guess, int algorithm,
                             doublereal tau[], doublereal I0[], doublereal beta[], doublereal R[], double gamma[],
                             doublereal t0[], doublereal offset[], doublereal scatter[], double tvb[], double ref_lifetime[],
                             int calculate_errs, doublereal tau_err[], doublereal beta_err[], doublereal R_err[],
                             doublereal offset_err[], doublereal scatter_err[], doublereal tvb_err[], doublereal ref_lifetime_err[],
                             doublereal chi2[], int ierr[],
                             int n_thread, int runAsync, int use_callback, int (*callback)())
{
   int error;

   error = CheckControllerIdx(c_idx);
   if (error)
      return error;

   if (!use_callback)
      callback = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( n_group, n_px, n_regions, global_mode,
                                      mask, n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      single_guess, tau_guess,
                                      fit_beta, fixed_beta,
                                      0, NULL,
                                      0, 0, 0, NULL,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, algorithm,
                                      tau, I0, beta, R, gamma, NULL, NULL, 
                                      t0, offset, scatter, tvb, ref_lifetime,
                                      calculate_errs, tau_err, beta_err, R_err, NULL, offset_err, 
                                      scatter_err, tvb_err, ref_lifetime_err,
                                      chi2, ierr,
                                      n_thread, runAsync, callback );

   error = controller[c_idx]->GetErrorCode();
   controller[c_idx]->SetData(data, data_type);
   controller[c_idx]->Init();

   error = controller[c_idx]->RunWorkers();
   return error;
   
}
           

FITDLL_API int FLIMGlobalPolarisationFit(int c_idx, int n_group, int n_px, int n_regions[], int global_mode,
                             int data_type, double data[], int mask[],
                             int n_t, double t[],
                             int n_irf, double t_irf[], double irf[], double pulse_pileup,
                             int n_exp, int n_fix, 
                             double tau_min[], double tau_max[], 
                             int single_guess, double tau_guess[],
                             int fit_beta, double fixed_beta[],
                             int use_magic_decay, double magic_decay[],
                             int n_theta, int n_theta_fix, int inc_rinf, double theta_guess[],
                             int fit_t0, double t0_guess,
                             int fit_offset, double offset_guess, 
                             int fit_scatter, double scatter_guess,
                             int fit_tvb, double tvb_guess, double tvb_profile[],
                             int pulsetrain_correction, double t_rep,
                             int ref_reconvolution, double ref_lifetime_guess, int algorithm,
                             double tau[], double I0[], double beta[], double theta[], double r[], 
                             double t0[], double offset[], double scatter[], double tvb[], double ref_lifetime[],
                             int calculate_errs, doublereal tau_err[], doublereal beta_err[], doublereal theta_err[],
                             doublereal offset_err[], doublereal scatter_err[], doublereal tvb_err[], doublereal ref_lifetime_err[],
                             double chi2[], int ierr[],
                             int n_thread, int runAsync, int use_callback, int (*callback)())
{

   int error = CheckControllerIdx(c_idx);
   if (error)
      return error;

   if (!use_callback)
      callback = NULL;

   int n_fret = 0;
   int n_fret_fix = 0;
   int inc_Rinf = 0;
   double* R_guess = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( n_group, n_px, n_regions, global_mode,
                                      mask, n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      single_guess, tau_guess,
                                      fit_beta, fixed_beta,
                                      use_magic_decay, magic_decay,
                                      n_theta, n_theta_fix, inc_rinf, theta_guess,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, algorithm,
                                      tau, I0, beta, NULL, NULL, theta, r,
                                      t0, offset, scatter, tvb, ref_lifetime, 
                                      calculate_errs, tau_err, beta_err, NULL, theta_err, offset_err, 
                                      scatter_err, tvb_err, ref_lifetime_err,
                                      chi2, ierr,
                                      n_thread, runAsync, callback );

   controller[c_idx]->SetPolarisationMode(MODE_POLARISATION);
   controller[c_idx]->SetData(data, data_type);
   controller[c_idx]->Init();
   
   error = controller[c_idx]->GetErrorCode();
   if (error)
      return error;

   error = controller[c_idx]->RunWorkers();
   return error;

}

/*
FITDLL_API int FLIMPolarisationSimulation(int n_px,
                             int n_t, double t[],
                             int n_irf, double t_irf[], double irf[], double pulse_pileup,
                             int n_exp, int n_fix, 
                             double tau[], double beta[],
                             int n_theta, int inc_rinf, double theta_guess[],
                             double t0_guess,
                             int fit_offset, double offset_guess, 
                             int fit_scatter, double scatter_guess,
                             int fit_tvb, double tvb_guess, double tvb_profile[],
                             int pulsetrain_correction, double t_rep,
                             int ref_reconvolution, double ref_lifetime_guess, int algorithm,
                             
                             int n_thread, int runAsync, int use_callback, int (*callback)())
{

   int error = CheckControllerIdx(c_idx);
   if (error)
      return error;

   if (!use_callback)
      callback = NULL;

   int n_fret = 0;
   int n_fret_fix = 0;
   int inc_Rinf = 0;
   double* R_guess = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( n_group, n_px, n_regions, global_mode,
                                      mask, n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      single_guess, tau_guess,
                                      fit_beta, fixed_beta,
                                      use_magic_decay, magic_decay,
                                      n_theta, n_theta_fix, inc_rinf, theta_guess,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, algorithm,
                                      tau, I0, beta, NULL, NULL, theta, r,
                                      t0, offset, scatter, tvb, ref_lifetime, 
                                      calculate_errs, tau_err, beta_err, NULL, theta_err, offset_err, 
                                      scatter_err, tvb_err, ref_lifetime_err,
                                      chi2, ierr,
                                      n_thread, runAsync, callback );

   controller[c_idx]->SetPolarisationMode(MODE_POLARISATION);
   controller[c_idx]->SetData(data, data_type);
   controller[c_idx]->Init();
   
   error = controller[c_idx]->GetErrorCode();
   if (error)
      return error;

   error = controller[c_idx]->RunWorkers();
   return error;

}

*/

FITDLL_API int FLIMGlobalGetChi2Map(int c_idx, int data_type, double data[], int n_t, double t[],
                                    int n_irf, double t_irf[], double irf[], double pulse_pileup,
                                    int n_exp, int n_fix, 
                                    double tau_min[], double tau_max[], double tau_guess[],
                                    int fit_beta, double fixed_beta[],
                                    int fit_t0, double t0_guess, 
                                    int fit_offset, double offset_guess, 
                                    int fit_scatter, double scatter_guess,
                                    int fit_tvb, double tvb_guess, double tvb_profile[],
                                    int n_fret, int n_fret_fix, int inc_Rinf, double R_guess[],
                                    int pulsetrain_correction, double t_rep,
                                    int ref_reconvolution, double ref_lifetime_guess,
                                    int grid_size, double grid[], 
                                    double tau[], double I0[], double beta[], double R[], double gamma[],
                                    double t0[], double offset[], double scatter[], double tvb[], double ref_lifetime[],
                                    double chi2[],
                                    int runAsync, int use_callback, int (*callback)())
{

   //DWORD wait_result = WaitForSingleObject(start_mutex,1000);

   int n_regions = 1;
   int mask = 1;
   int ierr = 0;
   int n_thread = 1;

   int error = CheckControllerIdx(c_idx);
   if (error) return error;

   if (!use_callback)
      callback = NULL;

   controller[c_idx] = 
         new FLIMGlobalFitController( 1, 1, &n_regions, 0,
                                      &mask,
                                      n_t, t,
                                      n_irf, t_irf, irf, pulse_pileup,
                                      n_exp, n_fix, tau_min, tau_max, 
                                      1, tau_guess,
                                      fit_beta, fixed_beta,
                                      0, NULL,
                                      0, 0, 0, NULL,
                                      fit_t0, t0_guess, 
                                      fit_offset, offset_guess, 
                                      fit_scatter, scatter_guess,
                                      fit_tvb, tvb_guess, tvb_profile,
                                      n_fret, n_fret_fix, inc_Rinf, R_guess, 
                                      pulsetrain_correction, t_rep,
                                      ref_reconvolution, ref_lifetime_guess, 3,
                                      tau, I0, beta, R, gamma, NULL, NULL, t0, offset, 
                                      scatter,tvb, ref_lifetime, 
                                      0, NULL, NULL, NULL, NULL, NULL, 
                                      NULL, NULL, NULL,
                                      chi2, &ierr,
                                      n_thread, runAsync, callback );

   error = controller[c_idx]->GetErrorCode();
   controller[c_idx]->SetData(data, data_type);
   controller[c_idx]->SetChi2MapMode(grid_size, grid);
   controller[c_idx]->Init();

   error = controller[c_idx]->RunWorkers();

   return error;
}

FITDLL_API int FLIMGlobalGetFit(int c_idx, int ret_group_start, int n_ret_groups, int n_fit, int fit_mask[], int n_t, double t[], double fit[])
{

   int valid = ValidControllerIdx(c_idx);
   if (!valid) return ERR_NO_FIT;

   int error = controller[c_idx]->GetFit(ret_group_start,n_ret_groups,n_fit,fit_mask,n_t,t,fit); 
   return error;
   
   return 0;
}

FITDLL_API int FLIMGlobalClearFit(int c_idx)
{

   if (c_idx >= 0)
   {
      int valid = ValidControllerIdx(c_idx);
      if (valid)
         delete controller[c_idx];
   }
   else
   {
      for(int i=1; i<MAX_CONTROLLER_IDX; i++)
      {
         if (controller[i] != NULL)
            delete controller[i];
      }
   }

   return SUCCESS;
}


FITDLL_API int FLIMGetFitStatus(int c_idx, int *group, int *n_completed, int *iter, double *chi2, double *progress)
{  

   int valid = ValidControllerIdx(c_idx);
   if (!valid)
      return ERR_NOT_INIT;

   controller[c_idx]->status->CalculateProgress();

   for(int i=0; i<controller[c_idx]->status->n_thread; i++)
   {
      group[i]       = controller[c_idx]->status->group[i]; 
      n_completed[i] = controller[c_idx]->status->n_completed[i];
      iter[i]        = controller[c_idx]->status->iter[i];
      chi2[i]        = controller[c_idx]->status->chi2[i];
   }
   *progress = controller[c_idx]->status->progress;

   return controller[c_idx]->status->Finished();
}


FITDLL_API int FLIMGlobalTerminateFit(int c_idx)
{
   int valid = ValidControllerIdx(c_idx);
   if (!valid)
      return ERR_NOT_INIT;

   controller[c_idx]->status->Terminate();
   return SUCCESS;
}