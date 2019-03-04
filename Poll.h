#ifndef _POLL_H_
#define _POLL_H_
#include <vector>
#include <string>
#include <ctime>


class CPoll
{
public:

    CPoll(std::string name, std::string node, std::string content);

    std::string GetName();                      //zwraca nazwe glosowania

    int CheckStat(int v);                       //sprawdzenie statusu glosowania, zwraca 0, gdy glosowanie w toku, liczbe dodatnia jesli Y i ujemna jesli N


    std::string Result();                       //zwraca wynik glosowania

    void Vote(bool yn, std::string voter);      //zwraca true, jesli glos oddany poprawnie

private:

    std::string                         m_name;     //nazwa glosowania
    std::string                         m_node;     //nazwa wezla inicjujacego
    std::string                         m_content;  //tresc glosowania
    //int                               m_n;        //liczba glosujacych
    std::vector<std::pair<bool, std::string>>    m_voted;      //paruje oddane glosy i ich odpowiedzi
    int                                 m_stat;       //aktualny stan glosowania - glos na tak inkrementuje, glos na nie dekrementuje

    time_t                              m_start;    //czas rozpoczecia glosowania
};

#endif
