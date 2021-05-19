#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "Vec.hpp"
#include "VecN.hpp"
#include <iostream>

TRE_NS_START

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
class Matrix
{
private:
	TYPE determinant_helper(TYPE m[R][C], uint8);
public:
	typedef Matrix class_type;
	typedef TYPE value_type;
	union
	{
		TYPE m[R][C];
		TYPE data[R*C];
	};
	

	FORCEINLINE Matrix();

	FORCEINLINE Matrix(bool);

	template<typename U>
	FORCEINLINE Matrix(U);

	FORCEINLINE Matrix(const Matrix<TYPE, proc, R, C>&);

	FORCEINLINE Matrix(const TYPE arr[R][C]);

	//Destructor
	FORCEINLINE ~Matrix() {};

	FORCEINLINE TYPE determinant();

	FORCEINLINE void print();

	FORCEINLINE Matrix<TYPE, proc, R, C>& operator=(const Matrix<TYPE, proc, R, C>&);

	FORCEINLINE Matrix<TYPE, proc, C, R> transpose(){
		Matrix<TYPE, proc, C, R> tmat(false);
		for (uint8 i = 0; i < C; i++) {
			for (uint8 j = 0; j < R; j++) {
				tmat.m[i][j] = this->m[j][i];
			}
		}
		return tmat;
	}

	template<typename T, typename std::enable_if<!std::is_same<T, class_type>::value, void>::type>
	FORCEINLINE class_type& operator*=(T scalar)
	{
		TYPE s = static_cast<TYPE>(scalar);
		for (uint8 i = 0; i < R; i++) {
			for (uint8 j = 0; j < C; j++) {
				m[i][j] *= s;
			}
		}
		return *this;
	}

	FORCEINLINE class_type& operator*=(const class_type& mat)
	{
		class_type res = (*this) * mat;
		return *this = res;
	}

	template<typename T, typename std::enable_if<!std::is_same<T, class_type>::value, void>::type>
	friend FORCEINLINE class_type operator*(const class_type& mat, T scalar)
	{
		TYPE s = static_cast<TYPE>(scalar);
		Matrix<TYPE, proc, R, C> mat_res(false);
		for (uint8 i = 0; i < R; i++) {
			for (uint8 j = 0; j < C; j++) {
				mat_res.m[i][j] = mat.m[i][j] * s;
			}
		}
		return mat_res;
	}

	template<typename T, typename std::enable_if<!std::is_same<T, class_type>::value, void>::type>
	friend FORCEINLINE class_type operator*(T scalar, const class_type& mat)
	{
		TYPE s = static_cast<TYPE>(scalar);
		Matrix<TYPE, proc, R, C> mat_res(false);
		for (uint8 i = 0; i < R; i++) {
			for (uint8 j = 0; j < C; j++) {
				mat_res.m[i][j] = s * mat.m[i][j];
			}
		}
		return mat_res;
	}

	template<uint8 C2>
	friend FORCEINLINE Matrix<TYPE, normal, R, C2> operator*(const class_type& mat1, const Matrix<TYPE, normal, C, C2>& mat2)
	{
		Matrix<TYPE, normal, R, C2> mat_res(0);
		for (uint8 i = 0; i < R; i++) {
			for (uint8 j = 0; j < C2; j++) {
				for (uint8 k = 0; k < C; k++) {
					mat_res.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
				}
			}
		}
		return mat_res;
	}

	friend FORCEINLINE Vec<R, TYPE, proc> operator*(const class_type& mat, const Vec<R, TYPE, proc>& v1)
	{
		Vec<R, TYPE, normal> vec;
		for (uint8 j = 0; j < C; j++) {
			for (uint8 i = 0; i < R; i++) {
				vec.data[i] += mat.m[j][i] * v1.data[i];
			}
		}
		return vec;
	}

	friend FORCEINLINE Vec<R, TYPE, proc> operator*(const Vec<R, TYPE, proc>& v1, const class_type& mat)
	{
		Vec<R, TYPE, normal> vec;
		for (uint8 i = 0; i < R; i++) {
			for (uint8 j = 0; j < C; j++) {
				vec.data[j] += v1.data[i] * mat.m[i][j];
			}
		}
		return vec;
	}

#if __cplusplus <= 201103L
	FORCEINLINE Matrix(const std::initializer_list<std::initializer_list<TYPE>>&);
#endif
};

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix()
{
	for (uint8 i = 0; i < R; i++) {
		for (uint8 j = 0; j < C; j++) {
			if (i == j) {
				m[i][j] = 1;
			}else{
				m[i][j] = 0;
			}
		}
	}
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix(bool)
{
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix(const TYPE arr[R][C])
{
	for (uint8 i = 0; i < R; i++) {
		for (uint8 j = 0; j < C; j++) {
			m[i][j] = arr[i][j];
		}
	}
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
template<typename U>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix(U scalar)
{
	TYPE s = static_cast<TYPE>(scalar);
	for (uint8 i = 0; i < R; i++) {
		for (uint8 j = 0; j < C; j++) {
			m[i][j] = s;
		}
	}
}

#if __cplusplus <= 201103L
template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix(const std::initializer_list<std::initializer_list<TYPE>>& list)
{
	uint8 i = 0, j;
	for (const std::initializer_list<TYPE>& elm : list) {
		j = 0;
		for (const TYPE v : elm) {
			this->m[i][j] = v;
			j++;
		}
		i++;
	}
}
#endif

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE void Matrix<TYPE, proc, R, C>::print()
{
	for (uint8 i = 0; i < R; i++) {
		for (uint8 j = 0; j < C; j++) {
			//printf("%d", i);
			std::cout << m[i][j] << ", ";
		}
		std::cout << std::endl;
	}
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>& Matrix<TYPE, proc, R, C>::operator=(const Matrix<TYPE, proc, R, C>& other)
{
	memcpy(this, (const void*)(&other), sizeof(other));
	return *this;
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE Matrix<TYPE, proc, R, C>::Matrix(const Matrix<TYPE, proc, R, C>& other) {
	memcpy(this, (const void*)(&other), sizeof(other));
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
TYPE Matrix<TYPE, proc, R, C>::determinant_helper(TYPE m[R][C], uint8 n) {
	TYPE det = (TYPE)0, temp[R][C];
	int p, h, k, i, j;
	if (n == 1) {
		return m[0][0];
	}else if (n == 2) {
		det = (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
		return det;
	}else{
		for (p = 0; p < n; p++) {
			h = 0;
			k = 0;
			for (i = 1; i < n; i++) {
				for (j = 0; j < n; j++) {
					if (j == p) {
						continue;
					}
					temp[h][k] = m[i][j];
					k++;
					if (k == n - 1) {
						h++;
						k = 0;
					}
				}
			}
			det = det + m[0][p] * (int32)pow(-1, p) * determinant_helper(temp, n - 1);
		}
		return det;
	}
}

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
FORCEINLINE TYPE Matrix<TYPE, proc, R, C>::determinant() {
	if (R != C) { return (TYPE)0; }
	return determinant_helper(m, R);
}

template<typename T, usize l, usize c>
using Mat4 = TRE::Matrix<T, normal, l, c>;

TRE_NS_END