// Demonstrate use of Templates for simple summation
// ECE4893/8893  - Fall 2011
// George F. Riley, Georgia Tech, Fall 2011

#include <iostream>

using namespace std;

// Define class A with various constructors and addition/multiplaction operator
class A {
public:
  A();         // Default constructor
  A(int);      // Non-Default Constructor
  A(const A&); // A copy constructor is used by the compiler whenever
               // a "copy" of an object is needed.
  const A operator+(const A&) const;
  const A operator*(const A&) const;
  //A& operator+=(const A&);
  //A& operator*=(const A&);
  
public:
  int x;       // Single data member
};

// Implementation of class A
A::A()
{
}

A::A(int i)
    : x(i)
{
}

A::A(const A& a)
    : x(a.x)
{
}

// Implement the operators
const A A::operator+(const A& rhs) const
{
  return A(x + rhs.x);
}

const A A::operator*(const A& rhs) const
{
  return A(x * rhs.x);
}

//A& A::operator+=(const A& rhs)
//{
//  x += rhs.x;
//  return *this;
//}

//A& A::operator*=(const A& rhs)
//{
//  x *= rhs.x;
//  return *this;
//}

// Now define two functions to sum and product arrays of ints and doubles

// The summation functions
int SumInt(const int* p, unsigned length)
{
  int result = 0;
  for (int i = 0; i < length; ++i)
    {
      result += p[i];
    }
  return result;
}

int SumDouble(const double* p, unsigned length)
{
  double result = 0;
  for (int i = 0; i < length; ++i)
    {
      result += p[i];
    }
  return result;
}

// The product functions
int ProdInt(const int* p, unsigned length)
{
  int result = 1;
  for (int i = 0; i < length; ++i)
    {
      result *= p[i];
    }
  return result;
}

int ProdDouble(const double* p, unsigned length)
{
  double result = 1;
  for (int i = 0; i < length; ++i)
    {
      result *= p[i];
    }
  return result;
}

// Now, what if we want a summation/product of class A variables
A SumA(const A* p, unsigned length)
{
  A result = 0;
  for (int i = 0; i < length; ++i)
    {
      result += p[i];
    }
  return result;
}

A ProdA(const A* p, unsigned length)
{
  A result = 1;
  for (int i = 0; i < length; ++i)
    {
      result *= p[i];
    }
  return result;
}

// Also an output operator for A
ostream& operator<<(ostream& ofs, const A& rhs)
{
  ofs << rhs.x;
  return ofs; // This is important
}


// The above is silly; we wrote exactly the same code 6 times, the only
// difference is the type of the argument and return value.
// A C++ "Template" allows us to fix this problem.

template <typename T>
T SumTemplate(const T* p, unsigned length)
{
  T result = 0;
  for (int i = 0; i < length; ++i)
    {
      result += p[i];
    }
  return result;
}

template <typename T>
T ProdTemplate(const T* p, unsigned length)
{
  T result = 1;
  for (int i = 0; i < length; ++i)
    {
      result *= p[i];
    }
  return result;
}



int main()
{
  int    i[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  double d[10] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  A      a[10] = { A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9), A(10)};

  int    sumi = SumInt(   i, sizeof(i) / sizeof(int));
  double sumd = SumDouble(d, sizeof(d) / sizeof(double));
  A      sumA = SumA(     a, sizeof(a) / sizeof(A));
  
  int    prodi = ProdInt(   i, sizeof(i) / sizeof(int));
  double prodd = ProdDouble(d, sizeof(d) / sizeof(double));
  A      prodA = ProdA(     a, sizeof(a) / sizeof(A));

  // Now illustrate the use of the templated versions
  // This version uses "implicit instantiation
  int    sumit = SumTemplate(i, sizeof(i) / sizeof(int));
  double sumdt = SumTemplate(d, sizeof(d) / sizeof(double));
  A      sumAt = SumTemplate(a, sizeof(a) / sizeof(A));

  int    prodit = ProdTemplate(i, sizeof(i) / sizeof(int));
  double proddt = ProdTemplate(d, sizeof(d) / sizeof(double));
  A      prodAt = ProdTemplate(a, sizeof(a) / sizeof(A));

  // Print out the results
  cout << "sumi " << sumi << " sumit " << sumit << endl;
  cout << "sumd " << sumi << " sumdt " << sumdt << endl;
  cout << "sumA " << sumA << " sumAt " << sumAt << endl;
  
  cout << "prodi " << prodi << " prodit " << prodit << endl;
  cout << "prodd " << prodd << " proddt " << proddt << endl;
  cout << "proda " << prodA << " prodAt " << prodAt << endl;
  
}

  
