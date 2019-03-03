#ifndef _POLL_H_
#define _POLL_H_

#include<string>


class CPoll
{
public:

    Cpoll(std::string name, std::string node, int n);

    ~CPoll();

    std::string GetName();

    bool Result();              //false <=> N, true <=> Y

    void Vote(bool yn);

private:

    std::string     m_name;     //nazwa glosowania
    std::string     m_node;     //nazwa wezla inicjujacego
    int             n;          //liczba glosujacych
    int             m;          //liczba oddanych glosow
    int             stat;       //aktualny stan glosowania - glos na tak inkrementuje, glos na nie dekrementuje
};


class CNodeList
{
public:


private:

};




#endif
