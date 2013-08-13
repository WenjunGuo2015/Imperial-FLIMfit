//=========================================================================
//
// Copyright (C) 2013 Imperial College London.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// This software tool was developed with support from the UK 
// Engineering and Physical Sciences Council 
// through  a studentship from the Institute of Chemical Biology 
// and The Wellcome Trust through a grant entitled 
// "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).
//
// Author : Sean Warren
//
//=========================================================================

#ifndef _ABSTRACTFITTER_H
#define _ABSTRACTFITTER_H

#include "FitModel.h"
#include "FitResults.h"
#include "RegionData.h"

#include "omp_stub.h"
#include "levmar.h"

#include <cstdio>

class AbstractFitter
{
public:


   AbstractFitter(FitModel* model, int n_param, int max_region_size, int n_thread, int* terminate);
   virtual ~AbstractFitter();

   virtual int FitFcn(int nl, double *alf, int itmax, int max_jacb, int* niter, int* ierr) = 0;
   virtual int GetLinearParams(RegionData& results) = 0;
   
   int Fit(RegionData& region_data, FitResultsRegion& results, int thread, int itmax, int& niter, int &ierr, double& c2);
   int GetFit(int n_meas, int irf_idx, double* alf, float* lin_params, float* adjust, double* fit);
   double ErrMinFcn(double x);
   int CalculateErrors(double conf_limit);

   void GetParams(int nl, const double* alf);
   double* GetModel(const double* alf, int irf_idx, int isel, int thread);
   void ReleaseResidualMemory();

   int err;


protected:

   int Init();

   FitModel* model;

   int* terminate;

   double*  alf;
   double*  err_lower;
   double*  err_upper;

   // Used by variable projection
   int     inc[96];
   int     inc_full[96];
   int     ncon;
   int     nconp1;
   int     philp1;

   double *a_;
   double *r;
   double *b_;
   double *kap;
   double *params; 
   double *alf_err;
   double *alf_buf;

   int a_size;
   int b_size;

   //int     n;
   //int     s;
   //int     l;
   //int     lmax;
   int     n_param;
   //int     p;
   //int     pmax;

   int     max_region_size;
   //int     ndim;

   //int     lp1;

   float  *y;
   float  *avg_y;
   float *lin_params;
   float *chi2;
   //double *t;
   int    *irf_idx;

   double chi2_norm;
   double photons_per_count;
   double* cur_chi2;

   int n_thread;
   int variable_phi;

   int thread;

   int    fixed_param;
   double fixed_value_initial;
   double fixed_value_cur;
   double chi2_final;

   bool getting_errs;
   double conf_limit;

   int search_dir;

   FILE* f_debug;

};

#endif