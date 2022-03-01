#ifndef __TWO_D_ARRAY__
#define __TWO_D_ARRAY__

#include <algorithm>

/**
 *   @brief A class encapsulates a generic row-major 2d array.
 */
template<typename T>
class TwoDArray {
public:
    inline TwoDArray(int col=0, int row=0) {_init(col,row);}
    inline TwoDArray(int col, int row, T v) {_init(col,row); *this = v;}
    inline TwoDArray(const TwoDArray& ary) {
        _init(ary.m_Col, ary.m_Row);
        std::copy(ary.m_Data, ary.m_Data+(ary.m_Col*ary.m_Row), m_Data);
    }
    inline TwoDArray(int col,int row, T* vAry) {
        _init(col,row);
        std::copy(vAry, vAry+(col*row), m_Data);
    }
    inline virtual ~TwoDArray() {
        delete[] m_Data;
    }

    inline int GetColNum() const {return m_Col;}
    inline int GetRowNum() const {return m_Row;}

    TwoDArray& operator=(const TwoDArray &ary) {
        if(ary.m_Row != m_Row || ary.m_Col != m_Col) {
            delete[] m_Data;
            m_Row = ary.m_Row; m_Col = ary.m_Col;
            m_Data = new T[m_Row*m_Col];
        }
        std::copy(ary.m_Data, ary.m_Data+(m_Col*m_Row), m_Data);
        return *this;
    }

    TwoDArray& operator=(const T &v) {
        std::fill(m_Data,m_Data+(m_Col*m_Row),v);
        return *this;
    }
    inline T* operator[](int i) {return (m_Data+(m_Col*i));}
    inline T const * const operator[](int i) const {return (m_Data+(m_Col*i));}
    inline T& operator()(int i) {return m_Data[i];}
    inline const T& operator()(int i) const {return m_Data[i];}
    inline T& operator()(int c, int r) {return m_Data[r*m_Col+c];}
    inline const T& operator()(int c, int r) const {return m_Data[r*m_Col+c];}

    T* GetRawPtr() {return m_Data;}
    const T* GetRawPtr() const {return m_Data;}
private:

    void _init(int col, int row) {
        m_Col = col;
        m_Row = row;
        m_Data = new T[m_Col*m_Row];
    }

    int m_Col;
    int m_Row;
    T* m_Data;
};

#endif //#ifndef __TWO_D_ARRAY__
