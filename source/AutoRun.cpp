//
// Created by lsh on 20. 4. 4..
//

#include "AutoRun.h"
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include <mutex>

#include "Constant.h"

char e_dir[MaxProjectnum][250];
char mu_dir[MaxProjectnum][250];

//std::mutex mtx_e;
//std::mutex mtx_mu;

void AutoRun_e(int low, int high, int k)
{
    int error = 0;
    //FILE* fp_e = nullptr;

    for(int j = low; j < high; j++)
    {
        chdir(e_dir[j]);
        system("pwd");

        for(int i = 0; i < k; i++)
        {
            //mtx_e.lock();
            error = system("gnome-terminal -- sh -c \"./run && exit; bash\"");
            printf("||%d geant4_e- return %d\n", i, error);
            sleep(5);
            //mtx_e.unlock();
        }
    }

}

void AutoRun_mu(int low, int high, int k)
{
    int error = 0;
    for(int j = low; j < high; j++)
    {
        chdir(mu_dir[j]);
        system("pwd");

        for(int i = 0; i < k; i++)
        {
            //mtx_mu.lock();
            error = system("gnome-terminal -- sh -c \"./run && exit; bash\"");
            printf("||%d geant4_mu- return %d\n", i, error);
            sleep(5);
            //mtx_mu.unlock();
        }
    }

}

void ControlAutoRun(int number_of_proj, int repeat)
{
    const int threadnum = 4;
    using std::thread;
    thread threadlist_e[threadnum];
    thread threadlist_mu[threadnum];
    int works = 0;
    int w = 0;
    double energy;

    for(int i = 1; i <= number_of_proj; i++)
    {
        energy = (double)i / divide;
        sprintf(e_dir[i], "%s/e-/%fGeV/build", ProjectDir, energy);
        sprintf(mu_dir[i], "%s/mu-/%fGeV/build", ProjectDir, energy);
        //printf("%s\n", e_dir[i]);
        //printf("%s\n", mu_dir[i]);
    }

    printf(">>success to get build dir\n");

    works = number_of_proj / threadnum; //100 / 4
    if(works == 0)
    {
        printf("||thread: %d", number_of_proj);

        // for e-
        printf("|| simulate e-\n");
        for (int i = 0; i < number_of_proj; i++)
        {
            threadlist_e[i] = thread(AutoRun_e, i, i + 1, repeat);
            usleep(50);
        }
            


        for(int i = 0; i < number_of_proj; i++)
            threadlist_e[i].join();

        // for mu-
        printf("|| simulate mu-\n");
        for (int i = 0; i < number_of_proj; i++)
        {
            threadlist_mu[i] = thread(AutoRun_mu, i, i + 1, repeat);
            usleep(50);
        }
            

        for(int i = 0; i < number_of_proj; i++)
            threadlist_mu[i].join();

    }/////////////////////////////////////////////////////////////////////////////////
    else{
        printf("||works: %d\n", works);

        // for e-
        printf("|| simulate e-\n");
        for(int i = 0; i < threadnum - 1; i++)
        {
            w = i * works;
            threadlist_e[i] = thread(AutoRun_e, w, w + works, repeat);
            usleep(50);
        }
        threadlist_e[threadnum-1] = thread(AutoRun_e, w + works, number_of_proj + 1, repeat);

        for(auto & i : threadlist_e)
            i.join();

        sleep(1);
/////////////////////////////////////////////////////////////////////////////////////////////
        // for mu-
        printf("|| simulate mu-\n");
        for(int i = 0; i < threadnum - 1; i++)
        {
            w = i * works;
            threadlist_mu[i] = thread(AutoRun_mu, w, w + works, repeat);
            usleep(50);
        }
        threadlist_mu[threadnum-1] = thread(AutoRun_mu, w + works, number_of_proj + 1, repeat);

        for(auto & i : threadlist_mu)
            i.join();

    }


}
