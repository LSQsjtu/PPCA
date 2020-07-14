#ifndef PREDICT
#define PREDICT

#include <iostream>
#include <fstream>
using namespace std;

struct prediction
{
    int predict;

    prediction(int tmp = 1) : predict(tmp) {}

    void update(bool jump)
    {
        predict &= 3;
        if (predict == 3) // 11
            predict = jump ? 3 : 2;
        else if (predict == 2) // 10
            predict = jump ? 3 : 1;
        else if (predict == 1) // 01
            predict = jump ? 2 : 0;
        else // 00
            predict = jump ? 1 : 0;
    }
    bool get_predict(int pc);
} pred;
prediction history[0xff];

bool prediction::get_predict(int pc)
{
    int pos = (pc >> 2) & 0xff;

    if (history[pos].predict == 0 || history[pos].predict == 3)
    {
        return false;
    }
    else
    {
        return true;
    }
}

#endif