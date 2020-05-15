//
// Created by lsh on 20. 3. 4..
// save event during 0ns to 950ns
//

#include "Process.h"
#include <cstdio>
#include <mutex>
#include <algorithm>
#include "Constant.h"
#include "Class.h"
#include "FileIO.h"

extern FILE* fp_EdepData[Threadnum + 10];

extern char file_name[Filenum + 10][150];
extern FILE* fp_flux[Threadnum+10];

extern int state;

extern int number_of_file;

extern long long int number_of_event;

std::mutex mtx;
/*
inline void CopyTime(const Time& now, Time& old)
{
    old.year = now.year;
    old.month = now.month;
    old.day = now.day;
    old.hour = now.hour;
    old.minute = now.minute;
    old.sec = now.sec;
}
*/
inline int CompareTime(const Time& now, const Time& old)
{
    if(now.year == old.year)
        if(now.month == old.month)
            if(now.day == old.day)
                if(now.hour == old.hour)
                    if(now.minute == old.minute)
                        if(now.sec == old.sec)
                            return 1;

    return 0;
}


void ProcessData(int threadnum, int name_index_low, int name_index_high)
{
    FILE* fp_data;
    int escape = 0;
    int temp[5] = { 0 };    // index, ch1, ch2, ch3, ch4
    Time now;
    Time old;
    long long int total_edep_ch2 = 0;
    long long int total_edep_ch3 = 0;
    int flux = 0;
    char datafile_temp_ch[300] = {0};
    int error = 0;

    printf("||thread %d: number of works: %d\n", threadnum, name_index_high - name_index_low);

    for (int k = name_index_low; k < name_index_high; k++)
    {
        sprintf(datafile_temp_ch, "%s/%s.txt",RawDataDir, file_name[k]);

        fp_data = fopen64(datafile_temp_ch, "r");

        if (fp_data != nullptr) // does program success to open data file?
        {
            while (!feof(fp_data) && escape < MaxParticlenum) // while end of file
            {
                fscanf(fp_data, "%d %d  %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);

                for (int i = 1; i <= 6; i++)
                {
                    fscanf(fp_data, "%d %d %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);

                    if(i == 1)
                    {
                        now.year = temp[2] * 256 + temp[3];
                        now.month = temp[4];
                    }
                    else if(i == 2)
                    {
                        now.day = temp[1];
                        now.hour = temp[2];
                        now.minute = temp[3];
                        now.sec = temp[4];
                    }
                } // time data end

                // consider if the time data have problem
                if(now.year > 2020 || now.month > 12 || now.day > 31 || now.hour > 24 || now.minute > 59 || now.sec > 59)
                {
                    //printf(">>thread %d: time information is wrong in file %s\r", threadnum, file_name[k]);
                    for (int i = 0; i <= 94; i++)
                    {
                        error = fscanf(fp_data, "%d %d %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
                        if(error)
                        { }

                    } // do nothing just reading
                }
                else //if there is no problem in time data
                {
                    for (int i = 0; i <= 94; i++) //read edep
                    {
                        error = fscanf(fp_data, "%d %d %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
                        if(error)
                        {
                            int regular[2];
                            regular[0] = (1024 - temp[2]) - 100;
                            regular[1] = (1024 - temp[3]) - 100;

                            if(regular[0] < 0) regular[0] = 0;
                            if(regular[1] < 0) regular[1] = 0;
                            total_edep_ch2 = total_edep_ch2 + regular[0];
                            total_edep_ch3 = total_edep_ch3 + regular[1];
                        }

                    }

                    // calculate flux
                    if(escape > 0)
                    {
                        if(CompareTime(now, old))
                            flux++;
                        else
                        {
                            old.ConvertTime();
                            fprintf(fp_flux[threadnum], "%llu,%d\n",old.time, flux+1);
                            flux = 0;
                        }
                    }

                    //old_time = time;
                    //CopyTime(now, old);
                    old = now;

                    mtx.lock();
                    number_of_event++;
                    mtx.unlock();

                    // save processed data
                    now.ConvertTime();
                    fprintf(fp_EdepData[threadnum], "%llu %lld %lld\n", now.time, total_edep_ch2, total_edep_ch3);

                } // end of reading one event

                escape++;

                total_edep_ch2 = 0;
                total_edep_ch3 = 0;

            } // reading end
            fclose(fp_data);

            escape = 0;
            //index_for_flux = 0;

        } // fp_data is available
        else // fail to open data file
            printf(">>thread:%d error!! Can't open Data file\n", threadnum);

        mtx.lock();
        state++;
        printf(">>step 1: %f %% end\t\r", (float)state/(float)(number_of_file) * 100);
        mtx.unlock();

    } // end of processing one file

    printf("||thread:%d end\n", threadnum);
}


bool cmp(const G4Data &p1, const G4Data &p2)
{
    return p1.energy < p2.energy;
}

G4Data g4data[MaxEdepNumber];
void SortSimulationData()
{
    FILE* fp_mu;
    FILE* fp_e;
    int escape;
    char dir[150];

    printf("||sort mu- data \n");
    sprintf(dir, "%s/simulation_mu.txt", SimulationDataDir);
    fp_mu = fopen64(dir, "r");

    escape = 0;
    if(fp_mu != nullptr)
    {
        while(!feof(fp_mu) && escape < MaxEdepNumber)
        {
            fscanf(fp_mu, "%lf %lf %lf", &g4data[escape].energy, &g4data[escape].total_edep_ch2, &g4data[escape].total_edep_ch3);
            escape++;
        }
        std::sort(g4data, g4data + escape - 1, cmp);

        fclose(fp_mu);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //sprintf(dir, "%s/simulation_mu.txt", SimulationDataDir);
        fp_mu = fopen64(dir, "w");

        if(fp_mu != nullptr)
        {
            for(int i = 0; i < escape; i++)
                fprintf(fp_mu, "%f %lf %lf\n", g4data[i].energy, g4data[i].total_edep_ch2, g4data[i].total_edep_ch3);

            fclose(fp_mu);
        }
        else{
            printf(">>error occur while making simulation_mu.txt - sorted\n");
        }
    }
    else{
        printf(">>error occur while opening simulation_mu.txt\n");
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    printf("||sort e- data\n");
    sprintf(dir, "%s/simulation_e.txt", SimulationDataDir);
    fp_e = fopen64(dir, "r");

    escape = 0;
    if(fp_e != nullptr)
    {
        while(!feof(fp_e) && escape < MaxEdepNumber)
        {
            fscanf(fp_e, "%d %lf %lf %lf", &g4data[escape].particle, &g4data[escape].energy, &g4data[escape].total_edep_ch2, &g4data[escape].total_edep_ch3);
            escape++;
        }
        std::sort(g4data, g4data + escape - 1, cmp);

        fclose(fp_e);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //sprintf(dir, "%s/simulation_e.txt", SimulationDataDir);
        fp_e = fopen64(dir, "w");

        if(fp_e != nullptr)
        {
            for(int i = 0; i < escape; i++)
                fprintf(fp_e, "%lf %lf %lf\n", g4data[i].energy, g4data[i].total_edep_ch2, g4data[i].total_edep_ch3);

            fclose(fp_e);
        }
        else{
            printf(">>error occur while making simulation_e.txt - sorted\n");
        }
    }
    else{
        printf(">>error occur while opening simulation_e.txt\n");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FILE* fp_divide[MaxFluxIn1Hour];

int DivideEdepByHour(int hour)
{
    auto* data_buffer = new ObData[MaxFluxIn1Hour];
    FILE* fp_data;
    FILE* fp_divide;
    char dir[250];
    int index = 0;
    unsigned long long int start_time;
    int number_of_divide_file = 0;
    char edep_dir[150] = {0};
    int err;

    sprintf(dir, "%s/ObEdep.txt", ProcessedDataDir);

    fp_data = fopen64(dir, "r");

    if(fp_data != nullptr)
    {
        while(!feof(fp_data))
        {
            err = fscanf(fp_data, "%lld %d %d", &data_buffer[index].time, &data_buffer[index].total_edep_ch2, &data_buffer[index].total_edep_ch3);

            if(err == EOF)
            {
                printf("error occur when index is %d", index);
                break;
            }
            else{
                start_time = data_buffer[0].time;

                if(data_buffer[index].time - start_time > hour * Hour)
                {
                    sprintf(edep_dir, "%s/_%d_edep.txt", DivideDataDir, number_of_divide_file);
                    fp_divide = fopen64(edep_dir, "w");
                    if(fp_divide != nullptr)
                    {
                        for(int i = 0; i < index; i++)
                            fprintf(fp_divide, "%lld %d %d\n", data_buffer[i].time, data_buffer[i].total_edep_ch2, data_buffer[i].total_edep_ch3);

                        fclose(fp_divide);
                    }
                    else{
                        printf(">>error occur while making divided edep file\n");
                    }

                    number_of_divide_file++;
                    printf(">> number of file %d\r", number_of_divide_file);
                    index = 0;
                }
                else{
                    index++;
                }
            }
        }

        fclose(fp_data);
    }

    return number_of_divide_file;
}




