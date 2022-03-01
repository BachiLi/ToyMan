#include "RIBLexer.h"
#include "RIBToken.h"

#include <cctype>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>

using namespace std;

namespace ToyMan {
namespace RIBParser {

RIBToken RIBLexer::NextToken() {    
    for(;m_InStream.good();) {  
        int peek = m_InStream.peek();
        if(peek == ' ' || peek == '\t') {
            m_InStream.ignore(1);
        } else if(peek == '\n') {
            line++;
            m_InStream.ignore(1);            
        } else if(peek == '#') {
            line++;
            m_InStream.ignore(numeric_limits<streamsize>::max(), '\n');            
        } else {
            break;
        }
    }

    if(m_InStream.eof())
        return RIBToken(RIBToken::END_OF_FILE);    

    int peek = m_InStream.peek();

    if(isdigit(peek) || peek == '.' || peek == '-' || peek == '+') {
        string str;
        while(isdigit(peek) || peek == '.' || peek == '-' || peek == '+' || peek == 'e') {
            str.push_back(peek);
            m_InStream.ignore(1);
            peek = m_InStream.peek();
        }
        stringstream ss(str);        
        float val;
        if((ss >> val).fail())
            throw UnknownToken();
        return RIBToken(RIBToken::NUMBER, val);        
    } else if(peek == '\"') {        
        m_InStream.ignore(1);
        string str;        
        while(m_InStream.good()) {
            int c = m_InStream.get();
            if(c == '\"') {
                return RIBToken(RIBToken::STRING, str);
            } else if(c == '\\') {
                m_InStream.get();
                int c = m_InStream.get();
                str.push_back(c);
            } else if(c == '\n') {
                throw UnterminatedStringException();
            } else {
                str.push_back(c);
            }
        }
        throw UnterminatedStringException();
    } else if(peek == '[') {
        m_InStream.ignore(1);
        return RIBToken(RIBToken::ARRAY_BEGIN);
    } else if(peek == ']') {        
        m_InStream.ignore(1);
        return RIBToken(RIBToken::ARRAY_END);
    } else if(isalpha(peek)) {
        string str;
        m_InStream >> str;
        return RIBToken(RIBToken::KEYWORD, str);
    }

    throw UnknownToken();
}

} //namespace ToyMan
} //namespace RIBParser