//
// Created by lsh on 20. 5. 5..
//
#include<cstdio>
#include <unistd.h>
#include "Constant.h"
#include "Class.h"
#include "Graph&Histogram.h"

Histogram_pack_int GetHistogram_observed(int hour)
{
    Histogram_pack_int pack;
    auto* data_buffer = new ObData[MaxFluxIn1Hour];
    FILE* fp_data;
    char dir[250];
    int index = 0;
    int hist = 0;
    unsigned long long int start_time;
    int err;

    sprintf(dir, "%s/ObEdep.txt", ProcessedDataDir);

    fp_data = fopen64(dir, "r");

    if(fp_data != nullptr)
    {
        printf("||observed\n");
        while(!feof(fp_data))
        {
            err = fscanf(fp_data, "%lld %d %d", &data_buffer[index].time, &data_buffer[index].total_edep_ch2, &data_buffer[index].total_edep_ch3);

            if(err == EOF)
            {
                printf("error occur when index is %d\n", index);
                break;
            }
            else{
                start_time = data_buffer[0].time;

                if(data_buffer[index].time - start_time > hour * Hour)
                {
                    pack.hist_ch2[hist].SetTime(start_time);
                    pack.hist_ch3[hist].SetTime(start_time);
                    pack.hist_total[hist].SetTime(start_time);

                    pack.hist_ch2[hist].GetHistogram();
                    pack.hist_ch3[hist].GetHistogram();
                    pack.hist_total[hist].GetHistogram();

                    index = 0;
                    pack.hist_ch2[hist].flush();
                    pack.hist_ch3[hist].flush();
                    pack.hist_total[hist].flush();
                    hist++;
                }
                else{
                    if(data_buffer[index].total_edep_ch2 != 0 && data_buffer[index].total_edep_ch3 != 0)
                    {
                        pack.hist_ch2[hist].ReadData(data_buffer[index].total_edep_ch2);
                        pack.hist_ch3[hist].ReadData(data_buffer[index].total_edep_ch3);
                        pack.hist_total[hist].ReadData(data_buffer[index].total_edep_ch2);
                        pack.hist_total[hist].ReadData(data_buffer[index].total_edep_ch3);
                    }
                    index++;
                }
            }
        }

        fclose(fp_data);
    }

    return pack;
}

Histogram_pack_double GetHistogram_simulation()
{
    Histogram_pack_double pack;
    FILE* fp_mu;
    FILE* fp_e;
    char dir[250];
    double energy;
    double energy_temp;
    double ch2, ch3;
    int index = 0;
    int hist = 0;

    sprintf(dir, "%s/simulation_mu.txt", SimulationDataDir);
    fp_mu = fopen64(dir, "r");

    sprintf(dir, "%s/simulation_e.txt", SimulationDataDir);
    fp_e = fopen64(dir, "r");

    if(fp_mu != nullptr)
    {
        printf("||particle: mu-\n");
        sleep(5);
        while(!feof(fp_mu))
        {
            fscanf(fp_mu, "%lf %lf %lf", &energy, &ch2, &ch3);

            if(index > 0)
            {
                if(energy_temp != energy)
                {
                    pack.hist_ch2_mu[hist].GetHistogram();
                    pack.hist_ch3_mu[hist].GetHistogram();
                    pack.hist_total_mu[hist].GetHistogram();

                    pack.hist_ch2_mu[hist].flush();
                    pack.hist_ch3_mu[hist].flush();
                    pack.hist_total_mu[hist].flush();

                    printf("||mu- energy: %lf\n", energy);
                    hist++;
                }
            }
            else{
                energy_temp = energy;
                pack.hist_ch2_mu[hist].ReadData(ch2);
                pack.hist_ch3_mu[hist].ReadData(ch3);
                pack.hist_total_mu[hist].ReadData(ch2);
                pack.hist_total_mu[hist].ReadData(ch3);

                printf("||hist = %d index = %d\r", hist, index);
                index++;
            }
        }

    }

    hist = 0;
    index = 0;
    if(fp_e != nullptr)
    {
        printf("||particle: e-\n");
        sleep(5);
        while(!feof(fp_e))
        {
            fscanf(fp_e, "%lf %lf %lf", &energy, &ch2, &ch3);

            if(index > 0)
            {
                if(energy_temp != energy)
                {
                    pack.hist_ch2_e[hist].GetHistogram();
                    pack.hist_ch3_e[hist].GetHistogram();
                    pack.hist_total_e[hist].GetHistogram();

                    pack.hist_ch2_e[hist].flush();
                    pack.hist_ch3_e[hist].flush();
                    pack.hist_total_e[hist].flush();

                    printf("||e- energy: %lf\n", energy);

                    hist++;
                }
            }
            else{
                energy_temp = energy;
                pack.hist_ch2_e[hist].ReadData(ch2);
                pack.hist_ch3_e[hist].ReadData(ch3);
                pack.hist_total_e[hist].ReadData(ch2);
                pack.hist_total_e[hist].ReadData(ch3);

                printf("||hist = %d index = %d\r", hist, index);
                index++;
            }
        }

    }

    return pack;
}
