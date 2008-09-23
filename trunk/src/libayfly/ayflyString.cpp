/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
 *   andrew@it-optima.ru                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ayfly.h"

size_t ayfly_strlen(const AY_CHAR *str)
{
#ifdef UNICODE
    return wcslen(str);
#else
    return strlen(str);
#endif
}

CayflyString::CayflyString()
{
    m_str = new AY_CHAR [1];
    m_str[0] = 0;

}

CayflyString::~CayflyString()
{
    if(m_str)
    {
        delete[] m_str;
        m_str = 0;
    }
}

CayflyString::CayflyString(const AY_CHAR *str)
{
    size_t len = ayfly_strlen(str);
    m_str = new AY_CHAR [len + 1];
    memcpy(m_str, str, len * sizeof(AY_CHAR));
    m_str[len] = 0;

}

CayflyString::CayflyString(const CayflyString & str)
{
    size_t len = ayfly_strlen(str.c_str());
    m_str = new AY_CHAR [len + 1];
    memcpy(m_str, str.c_str(), len * sizeof(AY_CHAR));
    m_str[len] = 0;
}

CayflyString& CayflyString::operator =(const CayflyString & str)
{
    return operator =(str.c_str());
}

CayflyString& CayflyString::operator =(const AY_CHAR *str)
{
    if(m_str)
    {
        delete[] m_str;
        m_str = 0;
    }
    size_t len = ayfly_strlen(str);
    m_str = new AY_CHAR [len + 1];
    memcpy(m_str, str, len * sizeof(AY_CHAR));
    m_str[len] = 0;
    return *this;
}

size_t CayflyString::length()
{
#ifdef UNICODE
    return wcslen(m_str);
#else
    return strlen(m_str);
#endif
}

const AY_CHAR *CayflyString::c_str()
{
    return m_str;
}

void CayflyString::toLower()
{
    size_t len = length();
    for(size_t i = 0; i < len; i++)
    {
        m_str [i] = tolower(m_str [i]);
    }
}



