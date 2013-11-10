/*
Copyright (C) 2010-2012 Kristian Duske

This file is part of TrenchBroom.

TrenchBroom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrenchBroom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TrenchBroom_Vec_h
#define TrenchBroom_Vec_h

#include "MathUtils.h"
#include "StringUtils.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <ostream>
#include <set>
#include <vector>

template <typename T, size_t S>
class Vec {
private:
    class SelectionHeapCmp {
    private:
        const Vec<T,S>& m_vec;
        bool m_abs;
    public:
        SelectionHeapCmp(const Vec<T,S>& vec, const bool abs) :
        m_vec(vec),
        m_abs(abs) {}
        
        bool operator()(size_t lhs, size_t rhs) const {
            assert(lhs < S);
            assert(rhs < S);
            if (m_abs)
                return std::abs(m_vec.v[lhs]) < std::abs(m_vec.v[rhs]);
            return m_vec.v[lhs] < m_vec.v[rhs];
        }
    };
    
    int weight(T c) const {
        if (std::abs(c - static_cast<T>(1.0)) < static_cast<T>(0.9))
            return 0;
        if (std::abs(c + static_cast<T>(1.0)) < static_cast<T>(0.9))
            return 1;
        return 2;
    }
public:
    static const Vec<T,S> PosX;
    static const Vec<T,S> PosY;
    static const Vec<T,S> PosZ;
    static const Vec<T,S> NegX;
    static const Vec<T,S> NegY;
    static const Vec<T,S> NegZ;
    static const Vec<T,S> Null;
    static const Vec<T,S> NaN;
    
    class LexicographicOrder {
    public:
        bool operator()(const Vec<T,S>& lhs, const Vec<T,S>& rhs) const {
            for (size_t i = 0; i < S; ++i) {
                if (Math::lt(lhs[i], rhs[i]))
                    return true;
                if (Math::gt(lhs[i], rhs[i]))
                    return false;
            }
            return false;
        }
    };

    class ErrorOrder {
    public:
        bool operator()(const Vec<T,S>& lhs, const Vec<T,S>& rhs) const {
            const T lErr = (lhs - lhs.rounded()).lengthSquared();
            const T rErr = (rhs - rhs.rounded()).lengthSquared();
            return lErr < rErr;
        }
    };
    
    class DotOrder {
    private:
        const Vec<T,S>& m_dir;
    public:
        DotOrder(const Vec<T,S>& dir) :
        m_dir(dir) {
            assert(!m_dir.null());
        }
        
        bool operator()(const Vec<T,S>& lhs, const Vec<T,S>& rhs) const {
            return lhs.dot(m_dir) < rhs.dot(m_dir);
        }
    };
    
    class InverseDotOrder {
    private:
        const Vec<T,S>& m_dir;
    public:
        InverseDotOrder(const Vec<T,S>& dir) :
        m_dir(dir) {
            assert(!m_dir.null());
        }
        
        bool operator()(const Vec<T,S>& lhs, const Vec<T,S>& rhs) const {
            return lhs.dot(m_dir) > rhs.dot(m_dir);
        }
    };
    
    typedef std::vector<Vec<T,S> > List;
    typedef std::set<Vec<T,S>, LexicographicOrder> Set;
    typedef std::map<Vec<T,S>, Vec<T,S>, LexicographicOrder> Map;
    
public:
    static const Vec<T,S> axis(const size_t index) {
        Vec<T,S> axis;
        axis[index] = static_cast<T>(1.0);
        return axis;
    }
    
    T v[S];
    
    Vec() {
        setNull();
    }
    
    Vec(const std::string& str) {
        setNull();
        
        const char* cstr = str.c_str();
        size_t pos = 0;
        std::string blank = " \t\n\r";
        
        for (size_t i = 0; i < S; ++i) {
            if ((pos = str.find_first_not_of(blank, pos)) == std::string::npos)
                return;
            v[i] = static_cast<T>(std::atof(cstr + pos));
            if ((pos = str.find_first_of(blank, pos)) == std::string::npos)
                return;
        }
    }
    
    Vec(const T i_x) {
        if (S > 0)
            v[0] = i_x;
        for (size_t i = 1; i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }
            
    Vec(const T i_x, const T i_y) {
        if (S > 0)
            v[0] = i_x;
        if (S > 1)
            v[1] = i_y;
        for (size_t i = 2; i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }
    
    Vec(const T i_x, const T i_y, const T i_z) {
        if (S > 0)
            v[0] = i_x;
        if (S > 1)
            v[1] = i_y;
        if (S > 2)
            v[2] = i_z;
        for (size_t i = 3; i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }
    
    Vec(const T i_x, const T i_y, const T i_z, const T i_w) {
        if (S > 0)
            v[0] = i_x;
        if (S > 1)
            v[1] = i_y;
        if (S > 2)
            v[2] = i_z;
        if (S > 3)
            v[3] = i_w;
        for (size_t i = 4; i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }

    template <typename U, size_t O>
    Vec(const Vec<U,O>& vec) {
        for (size_t i = 0; i < std::min(S,O); ++i)
            v[i] = static_cast<T>(vec[i]);
        for (size_t i = std::min(S,O); i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }

    template <typename U, size_t O>
    Vec(const Vec<U,O>& vec, const U last) {
        for (size_t i = 0; i < std::min(S-1,O); ++i)
            v[i] = static_cast<T>(vec[i]);
        for (size_t i = std::min(S-1, O); i < S-1; ++i)
            v[i] = static_cast<T>(0.0);
        v[S-1] = static_cast<T>(last);
    }
    
    template <typename U, size_t O>
    Vec(const Vec<U,O>& vec, const U oneButLast, const U last) {
        for (size_t i = 0; i < std::min(S-2,O); ++i)
            v[i] = static_cast<T>(vec[i]);
        for (size_t i = std::min(S-2, O); i < S-2; ++i)
            v[i] = static_cast<T>(0.0);
        v[S-2] = static_cast<T>(oneButLast);
        v[S-1] = static_cast<T>(last);
    }
    
    bool operator== (const Vec<T,S>& right) const {
        for (size_t i = 0; i < S; ++i)
            if (v[i] != right[i])
                return false;
        return true;
    }
    
    bool operator!= (const Vec<T,S>& right) const {
        return !(*this == right);
    }
    
    template <size_t O>
    Vec<T,S>& operator= (const Vec<T,O>& right) {
        for (size_t i = 0; i < std::min(S,O); ++i)
            v[i] = right[i];
        for (size_t i = std::min(S,O); i < S; ++i)
            v[i] = static_cast<T>(0.0);
        return *this;
    }
    
    const Vec<T,S> operator- () const {
        Vec<T,S> result;
        for (size_t i = 0; i < S; ++i)
            result[i] = -v[i];
        return result;
    }

    const Vec<T,S> operator+ (const Vec<T,S>& right) const {
        Vec<T,S> result;
        for (size_t i = 0; i < S; ++i)
            result[i] = v[i] + right[i];
        return result;
    }
    
    const Vec<T,S> operator- (const Vec<T,S>& right) const {
        Vec<T,S> result;
        for (size_t i = 0; i < S; ++i)
            result[i] = v[i] - right[i];
        return result;
    }
    
    const Vec<T,S> operator* (const T right) const {
        Vec<T,S> result;
        for (size_t i = 0; i < S; ++i)
            result[i] = v[i] * right;
        return result;
    }
    
    const Vec<T,S> operator/ (const T right) const {
        Vec<T,S> result;
        for (size_t i = 0; i < S; ++i)
            result[i] = v[i] / right;
        return result;
    }
    
    Vec<T,S>& operator+= (const Vec<T,S>& right) {
        for (size_t i = 0; i < S; ++i)
            v[i] += right[i];
        return *this;
    }
    
    Vec<T,S>& operator-= (const Vec<T,S>& right) {
        for (size_t i = 0; i < S; ++i)
            v[i] -= right[i];
        return *this;
    }
    
    Vec<T,S>& operator*= (const T right) {
        for (size_t i = 0; i < S; ++i)
            v[i] *= right;
        return *this;
    }
    
    Vec<T,S>& operator/= (const T right) {
        for (size_t i = 0; i < S; ++i)
            v[i] /= right;
        return *this;
    }
    
    T& operator[] (const size_t index) {
        assert(index < S);
        return v[index];
    }
    
    const T& operator[] (const size_t index) const {
        assert(index < S);
        return v[index];
    }
    
    T x() const {
        assert(S > 0);
        return v[0];
    }
            
    T y() const {
        assert(S > 1);
        return v[1];
    }
    
    T z() const {
        assert(S > 2);
        return v[2];
    }

    T w() const {
        assert(S > 3);
        return v[3];
    }
            
    Vec<T,2> xy() const {
        return Vec<T,2>(x(), y());
    }

    Vec<T,2> xz() const {
        return Vec<T,2>(x(), z());
    }

    Vec<T,2> yz() const {
        return Vec<T,2>(y(), z());
    }
    
    Vec<T,3> xyz() const {
        return Vec<T,3>(x(), y(), z());
    }
            
    Vec<T,4> xyzw() const {
        return Vec<T,4>(x(), y(), z(), w());
    }
            
    Vec<T,S-1> overLast() const {
        Vec<T,S-1> result;
        for (size_t i = 0; i < S-1; ++i)
            result[i] = v[i] / v[S-1];
        return result;
    }

    const T dot(const Vec<T,S>& right) const {
        T result = static_cast<T>(0.0);
        for (size_t i = 0; i < S; ++i)
            result += (v[i] * right[i]);
        return result;
    }
    
    T length() const {
        return std::sqrt(squaredLength());
    }
    
    T squaredLength() const {
        return dot(*this);
    }
    
    T distanceTo(const Vec<T,S>& other) const {
        return (*this - other).length();
    }
    
    T squaredDistanceTo(const Vec<T,S>& other) const {
        return (*this - other).squaredLength();
    }
    
    Vec<T,S>& normalize() {
        *this /= length();
        return *this;
    }
    
    const Vec<T,S> normalized() const {
        return Vec<T,S>(*this).normalize();
    }
    
    bool equals(const Vec<T,S>& other, const T epsilon = Math::Constants<T>::AlmostZero) const {
        for (size_t i = 0; i < S; ++i)
            if (std::abs(v[i] - other[i]) > epsilon)
                return false;
        return true;
    }
    
    bool null() const {
        return equals(Null, Math::Constants<T>::AlmostZero);
    }

    void setNull() {
        for (size_t i = 0; i < S; ++i)
            v[i] = static_cast<T>(0.0);
    }
            
    void set(const T value) {
        for (size_t i = 0; i < S; ++i)
            v[i] = value;
    }
    
    bool nan() const {
        for (size_t i = 0; i < S; ++i)
            if (!Math::isnan(v[i]))
                return false;
        return true;
    }
    
            bool parallelTo(const Vec<T,S>& other, const T epsilon = Math::Constants<T>::ColinearEpsilon) const {
        return std::abs(dot(other) - other.length()) <= epsilon;
    }
    
    int weight() const {
        return weight(v[0]) * 100 + weight(v[1]) * 10 + weight(v[2]);
    }
            
    size_t majorComponent(const size_t k) const {
        assert(k < S);
        
        if (k == 0) {
            size_t index = 0;
            for (size_t i = 1; i < S; ++i) {
                if (std::abs(v[i]) > std::abs(v[index]))
                    index = i;
            }
            return index;
        }
        
        // simple selection algorithm
        // we store the indices of the values in heap
        SelectionHeapCmp cmp(*this, true);
        std::vector<size_t> heap;
        for (size_t i = 0; i < S; ++i) {
            heap.push_back(i);
            std::push_heap(heap.begin(), heap.end(), cmp);
        }
        
        std::sort_heap(heap.begin(), heap.end(), cmp);
        return heap[S - k - 1];
    }

    const Vec<T,S> majorAxis(const size_t k) const {
        const size_t c = majorComponent(k);
        Vec<T,S> a = axis(c);
        if (v[c] < static_cast<T>(0.0))
            return -a;
        return a;
    }

    const Vec<T,S> absMajorAxis(const size_t k) const {
        const size_t c = majorComponent(k);
        return axis(c);
    }
    
    size_t firstComponent() const {
        return majorComponent(0);
    }
    
    size_t secondComponent() const {
        return majorComponent(1);
    }
    
    size_t thirdComponent() const {
        return majorComponent(2);
    }
    
    const Vec<T,3> firstAxis() const {
        return majorAxis(0);
    }
            
    const Vec<T,3> absFirstAxis() const {
        return absMajorAxis(0);
    }
    
    const Vec<T,3> secondAxis() const {
        return majorAxis(1);
    }
    
    const Vec<T,3> absSecondAxis() const {
        return absMajorAxis(1);
    }
    
    const Vec<T,3> thirdAxis() const {
        return majorAxis(2);
    }
    
    const Vec<T,3> absThirdAxis() const {
        return absMajorAxis(2);
    }
    
    void write(std::ostream& str) const {
        for (size_t i = 0; i < S; ++i) {
            str << v[i];
            if (i < S - 1)
                str << ' ';
        }
    }
    
    std::string asString() const {
        StringStream result;
        write(result);
        return result.str();
    }
            
    Vec<T,S>& makeAbsolute() {
        for (size_t i = 0; i < S; ++i)
            v[i] = std::abs(v[i]);
        return *this;
    }
            
    Vec<T,S> absolute() const {
        return Vec<T,S>(*this).makeAbsolute();
    }
    
    Vec<T,S>& round() {
        for (size_t i = 0; i < S; ++i)
            v[i] = Math::round(v[i]);
        return *this;
    }
    
    const Vec<T,S> rounded() const {
        return Vec<T,S>(*this).round();
    }
    
    bool isInteger(const T epsilon = Math::Constants<T>::AlmostZero) const {
        for (size_t i = 0; i < S; ++i)
            if (std::abs(v[i] - Math::round(v[i])) > epsilon)
                return false;
        return true;
    }
    
    Vec<T,S>& correct(const T epsilon = Math::Constants<T>::CorrectEpsilon) {
        for (size_t i = 0; i < S; ++i)
            v[i] = Math::correct(v[i], epsilon);
        return *this;
    }
    
    const Vec<T,S> corrected(const T epsilon = Math::Constants<T>::CorrectEpsilon) const {
        return Vec<T,S>(*this).correct();
    }
};
            
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::PosX = Vec<T,S>( static_cast<T>(1.0),  static_cast<T>(0.0),  static_cast<T>(0.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::PosY = Vec<T,S>( static_cast<T>(0.0),  static_cast<T>(1.0),  static_cast<T>(0.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::PosZ = Vec<T,S>( static_cast<T>(0.0),  static_cast<T>(0.0),  static_cast<T>(1.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::NegX = Vec<T,S>(-static_cast<T>(1.0),  static_cast<T>(0.0),  static_cast<T>(0.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::NegY = Vec<T,S>( static_cast<T>(0.0), -static_cast<T>(1.0),  static_cast<T>(0.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::NegZ = Vec<T,S>( static_cast<T>(0.0),  static_cast<T>(0.0), -static_cast<T>(1.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::Null = Vec<T,S>( static_cast<T>(0.0),  static_cast<T>(0.0),  static_cast<T>(0.0));
template <typename T, size_t S>
const Vec<T,S> Vec<T,S>::NaN  = Vec<T,S>(std::numeric_limits<T>::quiet_NaN(),
                                         std::numeric_limits<T>::quiet_NaN(),
                                         std::numeric_limits<T>::quiet_NaN());

typedef Vec<float,2> Vec2f;
typedef Vec<float,3> Vec3f;
typedef Vec<float,4> Vec4f;
typedef Vec<double,2> Vec2d;
typedef Vec<double,3> Vec3d;
typedef Vec<double,4> Vec4d;
            
template <typename T, size_t S>
Vec<T,S> operator*(const T left, const Vec<T,S>& right) {
    return Vec<T,S>(right) * left;
}

template <typename T>
Vec<T,3>& cross(Vec<T,3>& left, const Vec<T,3>& right) {
    return left = crossed(left, right);
}

template <typename T>
const Vec<T,3> crossed(const Vec<T,3>& left, const Vec<T,3>& right) {
    return Vec<T,3>(left[1] * right[2] - left[2] * right[1],
                    left[2] * right[0] - left[0] * right[2],
                    left[0] * right[1] - left[1] * right[0]);
}

template <typename T>
T angleBetween(const Vec<T,3> vec, const Vec<T,3>& axis, const Vec<T,3>& up) {
    // computes the CCW angle between axis and vector in relation to the given up vector
    // all vectors are expected to be normalized
    const T cos = vec.dot(axis);
    if (Math::eq(cos, static_cast<T>(1.0)))
        return static_cast<T>(0.0);
    if (Math::eq(cos, static_cast<T>(-1.0)))
        return Math::Constants<T>::Pi;
    const Vec<T,3> cross = crossed(axis, vec);
    if (cross.dot(up) >= static_cast<T>(0.0))
        return std::acos(cos);
    return Math::Constants<T>::TwoPi - std::acos(cos);
}

#endif