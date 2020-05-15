//
// Created by lsh on 20. 3. 4..
//

#ifndef DATAPROCESS_VER6L_FILEIO_H
#define DATAPROCESS_VER6L_FILEIO_H

#include "Constant.h"

void FileName(const char d[150], char (*file_name)[150], const char type[]);

int NumberOftxtFile(const char d[150]);

int NumberOfcsvFile(const char d[150]);

int OpenFile();

void CloseFile();

int reOpenFluxFile();

int ReadEdepFile();

//int OpenFileForSpline();

//int GetRunningDay(int nfilenum);

#endif //DATAPROCESS_VER6L_FILEIO_H
