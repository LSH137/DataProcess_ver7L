#include <iostream>
#include <cstdio>
#include <thread>
#include <unistd.h>
//#include <sys/resource.h>

#include "source/Constant.h"
#include "source/FileIO.h"
#include "source/Process.h"
#include "source/Gathering.h"
#include "source/Class.h"
#include "source/MakeDir.h"
#include "source/CreateFile.h"
#include "source/BuildG4Proj.h"
#include "source/AutoRun.h"
#include "source/Graph&Histogram.h"
using std::thread;
thread threadlist[Threadnum +10];

int number_of_file = 0;
int low = 0, high = 0;
int works = 0;
int state = 0;
char file_name[Filenum + 10][150];
char folderchk = 0;
long long int number_of_event = 0;

FILE* fp_EdepData[Threadnum + 10] = {nullptr};
FILE* fp_flux[Threadnum + 10] = {nullptr};
int number_of_proj = 0;

int main()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    printf("||122th test\n");
    printf("||make folder\n||%s/flux\n", ProcessedDataDir);
    printf("||%s/edep\n", ProcessedDataDir);
    printf("||%s/divide\n", DivideDataDir);
    printf("||%s/total\n", DivideDataDir);
    printf("||%s\n", ProjectDir);
    printf("||Data have to be at %s\n", RawDataDir);
    printf("||Root macro files must be at %s\n", RootMacroDir);
    printf("||Sample project must be at %s\n", SampleDir);
    printf("||number of file must more then %d\n", Threadnum);
    printf("||Are you ready?\n||press a to process all\n||press s for simulation\n||press c for sort simulation data\n||press h for draw histogram\n||press t for divide edep by 1hour and draw histogram\n||press q to quit\n>> ");
    fflush(stdin);

    scanf("%c", &folderchk);
    fflush(stdin);

    switch(folderchk)
    {
        case 'q': return 0;

        case 'a':
        {
            printf(">>step 1. process from raw data\n");

            if (OpenFile())
            {
                number_of_file = NumberOftxtFile(RawDataDir);
                printf("||number of file: %d\n", number_of_file);

                FileName(RawDataDir, file_name, TXT);

                if(number_of_file)
                    works = (int)(number_of_file / Threadnum);
                else
                {
                    printf(">>error. number_of_file is 0\n");
                    return 0;
                }

                printf("||works: %d\n", works);

                for (int i = 0; i < Threadnum - 1; i++)
                {
                    low = i * works;
                    high = low + works;

                    threadlist[i] = thread(ProcessData, i, low, high);
                    //sleep(1);
                }
                threadlist[Threadnum-1] = thread(ProcessData, Threadnum-1, high, number_of_file);


                for (int i = 0; i < Threadnum; i++)
                    threadlist[i].join();

                CloseFile();
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////
            printf(">>step 2. Gathering\n");
            printf("||number of event: %lld\n", number_of_event);

            Gathering();

            GatherEdep();
            ///////////////////////////////////////////////////////////////////////////////////////////////////


            ///////////////////////////////////////////////////////////////////////////////////////////////////

            //MixFluxAndCloud(cloud_data);
        }

        case 's':
        {
/*
            int max_energy;
            if(number_of_proj == 0) {
                printf(">>enter the maximum energy of particle\n>> ");
                fflush(stdin);
                scanf("%d", &max_energy);

                number_of_proj = max_energy * divide;
            }
*/
            int max_energy = 0;
            printf(">>step 3. make Geant4 project\n");
            printf("||enter the maximum energy of particle\n>> ");
            fflush(stdin);
            scanf("%d", &max_energy);

            chdir(ProjectDir);
            system("rm -rf e-");
            system("rm -rf mu-");

            number_of_proj = max_energy * divide;
            printf("||number of project: %d\n", number_of_proj);

            MakeDir(number_of_proj);
            MakeFile(number_of_proj);
            Build(number_of_proj);
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            int repeat = 0;
            printf(">>step 4. simulate\n");
            printf("||enter the number of particle for simulation: 10000 X ");
            fflush(stdin);
            scanf("%d", &repeat);
            sleep(1);
            ControlAutoRun(number_of_proj, repeat);
        }
        case 'c':
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            //int w;
            //printf(">>step 5. converting\n");
            ChangeCSVtoTXT();
            //scanf("%d", &w);
            //if(w == 1) printf("w is 1\n");
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            printf(">>step 6. sorting\n");
            SortSimulationData(); //number of data have to less than 500000
        }
        case 'h':
        {
            FILE* fp_root;
            printf(">>step 6. make histogram\n");

            chdir(RootMacroDir);
            fp_root = popen("root histogram.C", "w");
            if(fp_root != nullptr)
            {
                fprintf(fp_root, ".q");
                fclose(fp_root);
            }
            else{
                printf("error occur while execute histogram.C\n");
            }
        }
        case 't':
        {
            Histogram_pack_int hist_pack;
            Histogram_pack_double hist_simulation;

            hist_pack = GetHistogram_observed(1);
            hist_simulation = GetHistogram_simulation();

            /*
            int nfile = 0;
            FILE* fp;

            nfile = DivideEdepByHour(1);

            if(nfile != 0)
            {
                chdir(RootMacroDir);
                fp = popen("root histogram2.C", "w");

                if(fp != nullptr)
                {
                    fprintf(fp, "%d", nfile);
                    fprintf(fp, ".q");
                    pclose(fp);
                }
                else
                    printf("error occur!!\n");
            }


            */
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            printf("||process end\n");
            break;
        }
        default:
        {
            printf(">>error. you type wrong command\n");
            break;
        }
    }

    return 0;
}

