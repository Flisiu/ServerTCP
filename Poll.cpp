#include "Poll.h"

CPoll::CPoll(std::string name, std::string node, std::string content):m_name(name), m_node(node), m_content(content)
{
    m_stat=0;
    time(&m_start);
}


std::string CPoll::GetName()
{
    return m_name;
}

int CPoll::CheckStat(int v)
{
    int czas_glosowania = 450;

    if((v - m_voted.size())<abs(m_stat))       // sprawdzenie, czy nieoddanych glosow jest mniej niz wynosi przewaga ktorejs opcji
    {
        if(m_stat!=0)
        {
            return m_stat;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if(v==m_voted.size())
        {
            if(m_stat!=0)
            {
                return m_stat;
            }
            else
            {
                return -1;
            }
        }

        time_t now;
        time(&now);
        if(difftime(now, m_start) > czas_glosowania)
        {
            if(m_stat!=0)
            {
                return m_stat;
            }
            else
            {
                return -1;
            }
        }
    }
    return 0;
}

std::string CPoll::Result()
{
    if(m_stat>0)
    {
        return " Y";
    }
    else
    {
        return " N";
    }
}

void CPoll::Vote(bool yn, std::string voter)
{
    for(std::pair<bool,std::string> i: m_voted)
    {
        if(i.second==voter)
        {
            if(i.first==yn)
            {
                return;
            }
            else
            {
                if(yn)
                {
                    m_stat+=2;
                    i.first=yn;
                    return;
                }
                else
                {
                    m_stat-=2;
                    i.first=yn;
                    return;
                }
            }
        }
    }
    if(yn)
    {
        ++m_stat;
        m_voted.push_back(std::pair<bool,std::string>(yn,voter));
        return;
    }
    else
    {
        --m_stat;
        m_voted.push_back(std::pair<bool,std::string>(yn,voter));
        return;
    }
}
