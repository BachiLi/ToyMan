#ifndef __RIBPARSER_RIBTOKEN_H__
#define __RIBPARSER_RIBTOKEN_H__

#include <string>

using namespace std;

namespace ToyMan {
namespace RIBParser {

class RIBToken {
public:
    enum Tag {
        ARRAY_BEGIN, //[
        ARRAY_END,   //]
        STRING,      //"xxx"
        NUMBER,      //12.34
        KEYWORD,     //WorldBegin
        END_OF_FILE  //(eof)
    };

    Tag tag;
    RIBToken(Tag _tag)
        : tag(_tag) {}

    RIBToken(Tag _tag, const string &_str)
        : tag(_tag), str(_str) {}

    RIBToken(Tag _tag, float _num)
        : tag(_tag), num(_num) {}

    //a stupid approach, will waste some memory
    string str;
    float num;
};

} //namespace RIBParser
} //namespace ToyMan

#endif //#ifndef __RIBPARSER_RIBTOKEN_H__