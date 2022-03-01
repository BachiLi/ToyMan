#ifndef __RIBPARSER_RIB_LEXER_H__
#define __RIBPARSER_RIB_LEXER_H__

#include <iostream>
#include <exception>

using namespace std;

namespace ToyMan {
namespace RIBParser {

class RIBToken;

class UnterminatedStringException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Unterminated string detected.";
    }
};

class UnknownToken : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Unknown token detected.";
    }
};

class RIBLexer {
public:
    int line;

    RIBLexer(istream &in)
        : m_InStream(in), line(1) {}

    RIBToken NextToken();

private:
    istream &m_InStream;
};

}; //namespace RIBParser
}; //namespace ToyMan

#endif //#ifndef __RIBPARSER_RIB_LEXER_H__