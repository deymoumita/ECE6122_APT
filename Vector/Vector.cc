// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// MOUMITA DEY

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

	elements = (T*)malloc(reserved * sizeof(T));

	// call in place copy constructors to copy elements from rhs vector
	for (int i = 0; i < count; i++)
	{
		new (&elements[i]) T(rhs[i]);
	}
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
	elements = (T*) malloc (nReserved * sizeof(T));
	count = 0;
	reserved = nReserved;	
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
	elements = (T*) malloc (n * sizeof(T));
	count = n;
	reserved = n;
	for(int i = 0; i < count; i++)
	{
		new (&elements[i]) T(t);
	}
}

template <typename T>
void Vector<T>::Reserve(size_t n)
{
	elements = (T*) malloc (n * sizeof(T));
	count = 0;
	reserved = n;
}
#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
	// for the vector that goes out of scope in the end
	// reset the vector memory as is
	{
		for (int i = 0; i < count; i++)
		{
			elements[i].~T();
		}
		free(elements);
		count = 0;
		reserved = 0;
	}
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
	if (reserved > count)
	{
		new (&elements[count]) T(rhs);
		count++;
	}
	else
	{
		// allocate temporary memory of size + 1
		T* tempElements = (T*)malloc((count + 1) * sizeof(T));

		// call in place copy constructors to copy elements from elements to temporary constructor
		for (int i = 0; i < count; i++)
		{
			new (&tempElements[i]) T(elements[i]);
		}
		// now push the last new element rhs
		new (&tempElements[count]) T(rhs);
		// increment size of vector
		count++;
		reserved++;

		// free elements and assign temporary elements to elements
		for (int i = 0; i < count - 1; i++)
		{
			elements[i].~T();
		}	
		free(elements);
		elements = tempElements;			
	}
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
	// allocate temporary memory of size + 1
	T* tempElements = (T*)malloc((count + 1) * sizeof(T));

	// put the new element in the beginning
	new (&tempElements[0]) T(rhs);
	// call in place copy constructors to copy elements from elements to temporary constructor
	for (int i = 1; i < count + 1; i++)
	{
		new (&tempElements[i]) T(elements[i - 1]);
	}

	// free elements and assign temporary elements to elements
	for (int i = 0; i < count; i++)
	{
		elements[i].~T();
	}	
	free(elements);
	elements = tempElements;	
	// set size of vector accordingly
	count++;
	reserved++;
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
	if (count != 0)
	{
		elements[count-1].~T();
		count--;
		reserved--;
	}
	else
		free(elements);
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
	// allocate temporary memory of size - 1
	T* tempElements = (T*)malloc((count - 1) * sizeof(T));

	// call in place copy constructors to copy elements from elements to temporary constructor
	for (int i = 0; i < count - 1; i++)
	{
		new (&tempElements[i]) T(elements[i + 1]);
	}

	// set size of vector accordingly
	count--;
	reserved--;
	// free elements and assign temporary elements to elements
	for (int i = 0; i < count + 1; i++)
	{
		elements[i].~T();
	}	
	free(elements);
	elements = tempElements;	
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
	if(elements != NULL)
	{
		return elements[0];
	}
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
	if(elements != NULL)
	{
		return elements[count - 1];
	}
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{
	return elements[i];
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
	if (count == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
	for (int i = 0; i < count; i++)
	{
		elements[i].~T();
	}
	count = 0;
	reserved = 0;
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
	return VectorIterator<T>(elements + count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
	// no need to allocate any temporary memory
	// simply delete the it'th element and shift left remaining elements
	for (int i = 0; i < count - 1; i++)
	{
		if (&elements[i] >= it.current)	// it.current gives address of the it'th element
		{
			elements[i].~T();
			new (&elements[i]) T(elements[i + 1]);
		}
	}
	elements[count - 1].~T();
	count--;
	reserved--;
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
	// allocate temporary memory of size + 1
	T* tempElements = (T*)malloc((count + 1) * sizeof(T));
	for (int i = 0; i < count + 1; i++)
	{
		if (&elements[i] < it.current)	// it.current gives address of the it'th element
		{
			new (&tempElements[i]) T(elements[i]);
		}
		else if (&elements[i] == it.current)
		{

			new (&tempElements[i]) T(rhs);
		}
		else
		{
			new (&tempElements[i]) T(elements[i - 1]);
		}
	}
	for (int i = 0; i < count; i++)
	{
		elements[i].~T();
	}
	free(elements);
	elements = tempElements;
	count++;
	reserved++;
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
	current = NULL;
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{
	current = c;	
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
	current = rhs.current;
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
	return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{	
 	current += 1;
 	return current;
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{
	T* tempCurrent = current;
	current += 1;
	return tempCurrent;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
	if (current != rhs.current)
		return true;
	else
		return false;
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
	if (current != rhs.current)
		return false;
	else
		return true;	
}
