// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// YOUR NAME HERE

#include <iostream> // debugging
#include "Vector.h"

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector()
{
	elements = NULL;
	count = 0;
	reserved = 0;
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs)
{
	count = rhs.count;
	reserved = rhs.reserved;


}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ // Initialize with reserved memory
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
}

template <typename T>
void Vector<T>::Reserve(size_t n)
{
}
#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
	T* tempElements = (T*)malloc((count + 1) * sizeof(T));

	for (int i = 0; i < count; i++)
	{
		new (&tempElements[i]) T(elements[i]);
	}
	new (&tempElements[count]) T(rhs);
	count++;
	reserved++;
	for (int i = 0; i < count - 1; i++)
	{
		elements[i].~T();
	}	
	free(elements);
	elements = tempElements;	

}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
	T* tempElements = (T*)malloc((count + 1) * sizeof(T));

	for (int i = 0; i < count; i++)
	{
		new (&tempElements[i + 1]) T(elements[i]);
	}
	new (&tempElements[0]) T(rhs);
	count++;
	reserved++;
	for (int i = 0; i < count - 1; i++)
	{
		elements[i].~T();
	}	
	free(elements);
	elements = tempElements;	

}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{

}

template <typename T>
T& Vector<T>::operator[](size_t i)
{
	return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{
	return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
}




