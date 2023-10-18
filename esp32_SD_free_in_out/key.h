#ifndef _KEY_H
#define _KEY_H

#include <iostream>
#include <cstring>
#include <string>
using namespace std;

char key1[30] = "QASWZXDECVFRBNGTHYJUMKIOLP";       //�ܳ�
char key2[30] = "plijkomnhutybvfgcxerdwsqza";       //�ܳ�
char key3[35] = ")4(@;7/5<:3>2$,6'?9.1#8=+%-*0&";   //�ܳ�
char key4[10] = "\[_`]^";                           //�ܳ�

void closeeachkey(string pp);               //���ܺ���
void openeachkey(string pp);                //���ܺ���
string closekey(string pp, string prive);   //���ܺ���
string openkey(string pp, string prive);    //���ܺ���

string closekey(string pp, string prive)    //���ܹ���
{
    closeeachkey(prive);

    char message[80];
    int i;
    for (i = 0; i < pp.length(); i++)
        message[i] = pp[i];
    message[i] = '\0';
    int len = 0;                             //���Ƴ���
    len = strlen(message);
    for (int i = 0; i < len; i++)
    {
        if (message[i] >= 'A' && message[i] <= 'Z')
        {
            message[i] = key1[message[i] - 'A'];
        }
        else if (message[i] >= 'a' && message[i] <= 'z')
        {
            message[i] = key2[message[i] - 'a'];
        }
        else if (message[i] >= 35 && message[i] <= 64)
        {
            message[i] = key3[message[i] - '#'];
        }
        else if (message[i] >= 91 && message[i] <= 96)
        {
            message[i] = key4[message[i] - '['];
        }
    }
    openeachkey(prive);
    return message;
}

string openkey(string pp, string prive)             //���ܹ���
{
    closeeachkey(prive);

    char message[80];
    int i;
    for (i = 0; i < pp.length(); i++)
        message[i] = pp[i];
    message[i] = '\0';
    
    int key = 0;
    int len = strlen(message);   //���Ƴ���  

    for (int i = 0; i < len; i++)
    {
        if (message[i] >= 'A' && message[i] <= 'Z')
        {
            while (key1[key] != message[i])
            {
                ++key;
            }
            message[i] = key + 'A';
            key = 0;
        }
        else if (message[i] >= 'a' && message[i] <= 'z')
        {
            while (key2[key] != message[i])
            {
                ++key;
            }
            message[i] = key + 'a';
            key = 0;
        }
        else if (message[i] >= 35 && message[i] <= 64)
        {
            while (key3[key] != message[i])
            {
                ++key;
            }
            message[i] = key + '#';
            key = 0;
        }
        else if (message[i] >= 91 && message[i] <= 96)
        {
            while (key4[key] != message[i])
            {
                ++key;
            }
            message[i] = key + '[';
            key = 0;
        }
    }
    openeachkey(prive);
    return message;
}

void closeeachkey(string pp)
{
    int average = 0;
    int i;
    char temp = '/0';
    for (i = 0; i < pp.length(); i++)
        average += pp[i];
    average = average/(i-1);
    average = average%15;

    for (int j = 0; j < average&& j + average < strlen(key1); j++)
    {
        temp = key1[j];
        key1[j] = key1[j + average];
        key1[j + average] = temp;
    }
    for (int j = 0; j < average && j + average < strlen(key2); j++)
    {
        temp = key2[j];
        key2[j] = key2[j + average];
        key2[j + average] = temp;
    }
    for (int j = 0; j < average && j + average < strlen(key3); j++)
    {
        temp = key3[j];
        key3[j] = key3[j + average];
        key3[j + average] = temp;
    }
}

void openeachkey(string pp)
{
    int average = 0;
    int i;
    char temp = '/0';
    for (i = 0; i < pp.length(); i++)
        average += pp[i];
    average /= i - 1;
    average %= 15;

    for (int j = 0; j < average && j + average < strlen(key1); j++)
    {
        temp = key1[j + average];
        key1[j + average] = key1[j];
        key1[j] = temp;
    }
    for (int j = 0; j < average && j + average < strlen(key2); j++)
    {
        temp = key2[j + average];
        key2[j + average] = key2[j];
        key2[j] = temp;
    }
    for (int j = 0; j < average && j + average < strlen(key3); j++)
    {
        temp = key3[j + average];
        key3[j + average] = key3[j];
        key3[j] = temp;
    }
}
#endif