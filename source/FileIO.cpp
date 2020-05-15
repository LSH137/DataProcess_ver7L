//
// Created by lsh on 20. 3. 4..
//

#include "FileIO.h"
#include "Constant.h"
#include <cstdio>
#include <cstring>
#include <dirent.h>

//int runningDay[Daynum];
//extern int numberOfDay;

DIR *path;
char *p1,*p2;
int ret;
struct dirent *dir;

extern FILE* fp_EdepData[Threadnum + 10];

extern FILE* fp_re_EdepData[Threadnum + 10];

//extern char file_name[Filenum + 10][150];
extern  FILE* fp_flux[Threadnum + 10];

extern FILE* fp_re_flux[Threadnum];

/////////////////////////////////////////////////////////////////////////////////////////

int NumberOftxtFile(const char d[150])
{
    int name_index = 0;

    path = opendir(d);
    if (path)
    {
        while ((dir = readdir(path)) != nullptr)
        {
            p1 = strtok(dir->d_name,".");
            p2 = strtok(nullptr,".");

            if(p2 != nullptr)
            {
                ret = strcmp(p2,"txt");

                if(ret == 0)
                {
                    //printf("%d: %s\n",name_index + 1, p1);
                    name_index ++;
                }
            }

        }
        closedir(path);
    }
    return name_index;
}

int NumberOfcsvFile(const char d[150])
{
    //printf("number of csv file called\n");
    int name_index = 0;

    path = opendir(d);
    if (path)
    {
        while ((dir = readdir(path)) != nullptr)
        {
            p1 = strtok(dir->d_name,".");
            p2 = strtok(nullptr,".");

            if(p2 != nullptr)
            {
                ret = strcmp(p2,"csv");

                if(ret == 0)
                {
                    //printf("%d: %s\n",name_index + 1, p1);
                    name_index ++;
                }
            }

        }
        closedir(path);
    }
    printf("number of csv file: %d\n", name_index);
    return name_index;
}

void FileName(const char d[150], char(*file_name)[150], const char type[])
{
    int name_index = 0;

    //printf(">>success to enter FileName\n");

    path = opendir(d);
    if (path)
    {
        while ((dir = readdir(path)) != nullptr)
        {
            p1 = strtok(dir->d_name,".");
            p2 = strtok(nullptr,".");

            if(p2 != nullptr)
            {
                ret = strcmp(p2,type);

                if(ret == 0)
                {
                    for(int i = 0; p1[i] != '\0'; i++)
                        file_name[name_index][i] = p1[i];

                    name_index ++;
                }
            }

        }
        closedir(path);
    } else
        printf(">>error occur while get data file name\n");

    //printf("||FileName end\n");
}

int OpenFile()
{
    char processedData[150] = {0};
    char fluxData[150] = {0};

    for(int i = 0; i < Threadnum; i++)
    {
        sprintf(processedData, "%s/edep/_%d_edep.txt", ProcessedDataDir, i);
        fp_EdepData[i] = fopen64(processedData, "w");

        if(fp_EdepData[i] == nullptr)
        {
            for (int k = 0; k < i; k++)
                fclose(fp_EdepData[k]);

            printf(">>error occur while making edep data file!!\n");
            return 0;
        }
            //printf(">>success to make %d-edep data file\n", i);
    }
    printf(">>success to make edep data file\n");

    for(int i = 0; i < Threadnum; i++)
    {
        sprintf(fluxData, "%s/flux/_%d_flux.txt", ProcessedDataDir, i);
        fp_flux[i] = fopen64(fluxData, "w");

        if(fp_flux[i] == nullptr) {
            for (int k = 0; k < i; k++)
                fclose(fp_flux[k]);

            printf(">>error occur while making flux data file!!\n");
            return 0;
        }
    }

    printf(">>success to make flux data file\n");

    return 1;
}

void CloseFile()
{
    for (int i = 0; i < Threadnum; i++)
    {
        if (fp_EdepData[i] != nullptr)
            fclose(fp_EdepData[i]);
    }
    for (int i = 0; i < Threadnum; i++)
    {
        if (fp_flux[i] != nullptr)
            fclose(fp_flux[i]);
    }

}

int reOpenFluxFile()
{
    char fluxData[150] = {0};

    for(int i = 0; i < Threadnum; i++)
    {
        sprintf(fluxData, "%s/flux/_%d_flux.txt", ProcessedDataDir, i);
        fp_re_flux[i] = fopen64(fluxData, "r");

        if(fp_re_flux[i] == nullptr)
        {
            for (int k = 0; k < i; k++)
                fclose(fp_re_flux[k]);

            printf(">>error occur while opening flux data file!!\n");
            return 0;
        }
        //printf(">>success to make %d-edep data file\n", i);
    }
    printf(">>success to open flux data file\n");

    return 1;
}

int ReadEdepFile()
{
    char processedData[150] = { 0 };

    for (int i = 0; i < Threadnum; i++)
    {
        sprintf(processedData, "%s/edep/_%d_edep.txt", ProcessedDataDir, i);
        fp_re_EdepData[i] = fopen64(processedData, "r");

        if (fp_re_EdepData[i] == nullptr)
        {
            for (int k = 0; k < i; k++)
                fclose(fp_re_EdepData[k]);

            printf(">>error occur while opening edep data file!!\n");
            return 0;
        }
        //printf(">>success to make %d-edep data file\n", i);
    }
    printf(">>success to open edep data file\n");

    return 1;
}



