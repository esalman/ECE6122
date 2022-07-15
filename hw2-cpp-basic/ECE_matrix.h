/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  09/22/2019

Description:    This header file contains definitions for the class called ECE_Matrix 
                which holds a dynamic two dimensional array with M rows and N columns 
                of element type double. It supports all of the operations in the 
                attached file Hmk2_Test1.cpp.
*/

#include <iostream>

class ECE_Matrix
{
    int rows;
    int cols;
    double **matrix;
public:
    // constructors
    // default constructor
    ECE_Matrix();
    // other constructors
    ECE_Matrix( const int &x, const double &v );
    ECE_Matrix( const int &x, const int &y, const double &v );
    ECE_Matrix( const std::string &fname );

    // destructor
    virtual ~ECE_Matrix();

    // overloading
    // additions
    ECE_Matrix operator+( const double &num ) const;
    ECE_Matrix operator+( const ECE_Matrix &mat2 ) const;
    ECE_Matrix& operator+=( const ECE_Matrix &mat2 );
    friend ECE_Matrix operator+( const double &num, const ECE_Matrix &mat2 ) {
        return mat2 + num;
    };
    // subtraction
    ECE_Matrix operator-( const double &num ) const;
    ECE_Matrix operator-( const ECE_Matrix &mat2 ) const;
    ECE_Matrix& operator-=( const ECE_Matrix &mat2 );
    friend ECE_Matrix operator-( const double &num, const ECE_Matrix &mat2 ) {
        ECE_Matrix temp( mat2.rows, mat2.cols, num );
        return temp - mat2;
    };
    // multiplication
    ECE_Matrix operator*( const double &num ) const;
    friend ECE_Matrix operator*( const double &num, const ECE_Matrix &mat2 ) {
        return mat2 * num;
    };
    // division
    ECE_Matrix operator/( const double &num ) const;
    // copy assignment
    ECE_Matrix& operator=( const ECE_Matrix& );
    
    ECE_Matrix transpose() const;
    ECE_Matrix& resize( const int &x, const int &y );

    friend std::ostream& operator<<(std::ostream& os, const ECE_Matrix&);
};