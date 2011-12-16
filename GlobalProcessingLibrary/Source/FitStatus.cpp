#include "FitStatus.h"

FitStatus::FitStatus(FLIMGlobalFitController* gc, int n_group, int n_thread, int (*callback)()) : 
   gc(gc), n_group(n_group), n_thread(n_thread), callback(callback), 
   threads_running(0), progress(0), terminate(0), has_fit(0), running(0)
{
   group = new int[n_thread];
   n_completed = new int[n_thread];
   iter = new int[n_thread];
   chi2 = new double[n_thread];

   for(int i=0; i<n_thread; i++)
   {
      group[i] = 0;
      n_completed[i] = 0;

      iter[i] = 0;
      chi2[i] = 0;
   }

   started = false;

}

FitStatus::~FitStatus()
{
   delete[] group;
   delete[] iter;
   delete[] chi2;
   delete[] n_completed;

}

void FitStatus::FinishedGroup(int thread)
{
   n_completed[thread]++;
}

void FitStatus::AddThread()
{
   tthread::lock_guard<tthread::mutex> lock(running_mutex);
   started = true;
   running = true;
   threads_running++;
}

int FitStatus::RemoveThread()
{
   tthread::lock_guard<tthread::mutex> lock(running_mutex);
   --threads_running;
   has_fit = threads_running == 0;
   running = threads_running > 0;
   return threads_running;
}

void FitStatus::CalculateProgress()
{
   progress = 0;
   for(int i=0; i<n_thread; i++)
   {
      progress += n_completed[i]; 
   }
   progress = progress / (double) n_group;

}

int FitStatus::UpdateStatus(int thread, int t_group, int t_iter, double t_chi2)
{
   double progress = 0;

   if (t_group >= 0)
      group[thread] = t_group;
   iter[thread] = t_iter;
   chi2[thread] = t_chi2;//norm_chi2(gc, t_chi2, 1);

   for(int i=0; i<n_thread; i++)
   {
      progress += n_completed[i]; 
   }
   progress /= n_group;

   if (callback != 0)
   {
      int ret = 1; //callback(n_thread,n_completed,group,iter,chi2,progress);
      if (ret == 0)
         terminate = 1;
   }

   return terminate;
}

void FitStatus::Terminate()
{
   terminate = true;
}

bool FitStatus::Finished()
{
   if ( running_mutex.try_lock() )
   {
      bool ret = !running && started;
      running_mutex.unlock();
      return ret;
   }
   else 
      return false;
}

bool FitStatus::HasFit()
{
   if ( running_mutex.try_lock() )
   {
      bool ret =  has_fit && !running;
      running_mutex.unlock();
      return ret;
   }
   else 
      return false;
}

bool FitStatus::IsRunning()
{
   if ( running_mutex.try_lock() )
   {
      bool ret = running;
      running_mutex.unlock();
      return ret;
   }
   else
      return true;
}