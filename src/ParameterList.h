#ifndef __PARAMETER_LIST_H__
#define __PARAMETER_LIST_H__

#include "Point.h"
#include "Color.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace ToyMan {

class ParameterList {
public:
    template<typename T>
    inline T Find(const map<string, vector<T> > &m, const string &name, T defVal) const {
        map<string, vector<T> >::const_iterator it = m.find(name);
        if(it == m.end()) {
            return defVal;
        }
        vector<T> result = it->second;
        return result.size() > 0 ? result[0] : defVal;
    }

    template<typename T>
    inline vector<T> FindList(const map<string, vector<T> > &m, const string &name) const {
        map<string, vector<T> >::const_iterator it = m.find(name);
        if(m.find(name) == m.end()) {
            return vector<T>();
        }
        return it->second;
    }

    inline float FindFloat(const string &name, float defVal) const {
        return Find<float>(numberList, name, defVal);
    }    

    inline string FindString(const string &name, const string &defVal) const {
        return Find<string>(stringList, name, defVal);
    }

    inline Color FindColor(const string &name, const Color &defVal) const {
        vector<float> floats = FindFloatList(name);
        if(floats.size() != 3) return defVal;
        return Color(floats[0],floats[1],floats[2]);
    }

    inline Point FindPoint(const string &name, const Point &defVal) const {
        vector<float> floats = FindFloatList(name);
        if(floats.size() != 3) return defVal;
        return Point(floats[0],floats[1],floats[2]);
    }

    inline vector<float> FindFloatList(const string &name) const {
        return FindList<float>(numberList, name);
    }

    inline vector<string> FindStringList(const string &name) const {
        return FindList<string>(stringList, name);
    }

    inline vector<Point> FindPointList(const string &name) const {
        vector<float> floats = FindFloatList(name);
        vector<Point> points;
        if(floats.size() % 3 != 0) return points;
        for(size_t i = 0; i < floats.size(); i+=3) {
            points.push_back(Point(floats[i],floats[i+1],floats[i+2]));
        }
        return points;
    }

    inline vector<Normal> FindNormalList(const string &name) const {
    }

    map<string, vector<float> >  numberList;
    map<string, vector<string> > stringList;
};

}

#endif //#ifndef __PARAMETER_LIST_H__