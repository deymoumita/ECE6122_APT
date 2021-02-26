// Demonstrate the use of the Standard Template Library "vector" class.
// George F. Riley, ECE4493/8893 Georgia Tech, Fall 2012

// A vector is a variable length array.  It starts out as "zero" length
// and grows or shrinks as needed.  Further, the vector is a array
// of any arbitrary type, using the C++ "templates" feature.

#include <iostream>
#include <vector>
class A {
public:
  A();         // Default constructor
  A(int);      // Non-Default Constructor
  A(const A&); // A copy constructor is used by the compile whenever
  A& operator=(const A&); // Assignment operator
  ~A();        // Destructor
public:
  int x;       // Single data member
};


typedef std::vector<A>  AVec_t; // Define a type that is vector of A's
typedef std::vector<A*> APVec_t;// Define a type that is vector of A pointers

A::A()
{
  std::cout << "Hello from A::A() Default constructor" << std::endl;
}

A::A(int i)
    : x(i)
{
  std::cout << "Hello from A::A(int) constructor" << std::endl;
}

A::A(const A& a)
    : x(a.x)
{
  std::cout << "Hello from A Copy constructor" << std::endl;
}

A& A::operator=(const A& rhs)
{
  std::cout << "Hello from A Assignment operator" << std::endl;
  x = rhs.x;
}

A::~A()
{
  std::cout << "Hello from A Destructor" << std::endl;
}

//NewPage
int main()
{
  std::cout << "Creating A Vector"; getchar();
  AVec_t av0;
  
  std::cout << "Adding an three elements to av0"; getchar();
  av0.push_back(A(2));    // Elements are appended using "push_back"
  std::cout << "After first push_back"; getchar();
  av0.push_back(A(10));   // Elements are appended using "push_back"
  std::cout << "After second push_back"; getchar();
  av0.push_back(A(100));  // Elements are appended using "push_back"
  // Number of elements in a vector can be queried with "size()"
  std::cout << "After third push_back, size av0 is "<<av0.size()<<std::endl;

  // Now reserve space for  up to 10 elements, allowing for more
  // efficient push_back.
  std::cout << "Reserving 10 elements"; getchar();
  av0.reserve(10);
  // Push a few more to show better efficiency
  std::cout << "Pushing three more elements"; getchar();
  av0.push_back(A(101));
  av0.push_back(A(102));
  av0.push_back(A(103));
  
  // Individual elements can be accessed with the [] operator
  std::cout << "Accessing elements with the [] operator"; getchar();
  std::cout << "av0[0].x is " << av0[0].x << std::endl;
  std::cout << "av0[1].x is " << av0[1].x << std::endl;
  std::cout << "av0[2].x is " << av0[2].x << std::endl;

  // Front and back of list have special accessors
  std::cout << "Accessing elements with the front and back"; getchar();
  std::cout << "av0.front().x is " << av0.front().x << std::endl;
  std::cout << "av0.back().x is "  << av0.back().x << std::endl;

  // Vectors can be copied with copy constructor or assignment operator
  std::cout << "Making a copy of av0"; getchar();
  AVec_t av1(av0);
  std::cout << "Size of av1 is " << av1.size() << std::endl;
  std::cout << "av1[0].x is "    << av1[0].x << std::endl;
  
  // Vectors can be shrunk with "pop_back".  Notice that pop_back
  // does NOT return the element being popped
  std::cout << "Shrinking av0 with pop_back"; getchar();
  av0.pop_back(); // Remove last element
  std::cout << "Size of av0 is " << av0.size() << std::endl;
  av0.pop_back(); // Remove another element
  std::cout << "Size of av0 is " << av0.size() << std::endl;
  
  // Vectors can be initialized to "n" copies of a specified object
  std::cout << "Constructing  AVec_t with 10 elements"; getchar();
  AVec_t av2(10, A(1)); // Makes 10 elements of A(1)
  std::cout << "Size of av2 is " << av2.size() << std::endl;
  std::cout << "av2[0].x is " << av2[0].x << std::endl;
  
  // All elements of a vector can be removed with "clear()"
  std::cout << "Clearing av2"; getchar();
  av2.clear();
  std::cout << "Size of av2 is " << av2.size() << std::endl;

  // Push another element to demonstrate that "clear" did not
  // free the memory.
  std::cout << "push another on av2"; getchar();
  av2.push_back(A(100));
  
  std::cout << "push another on av2"; getchar();
  av2.push_back(A(200));
  
  // Create and populate a vector of A poiners
  std::cout << "Creating A Pointer Vector"; getchar();
  APVec_t apv0;
  
  std::cout << "Adding an three elements to apv0"; getchar();
  apv0.push_back(new A(2));
  apv0.push_back(new A(10));
  apv0.push_back(new A(100));
  // Number of elements in a vector can be queried with "size()"
  std::cout << "Size of apv0 is " << apv0.size() << std::endl;
  
  // Clear the apv0 vector.  Note: ~A() NOT called.  Why not?
  std::cout << "Clearing apv0"; getchar();
  apv0.clear();
  std::cout << "Size of apv0 is " << apv0.size() << std::endl;
  
  std::cout << "Main program exiting"; getchar();
  return 0;
}

// Output from this program is:
//
// Creating A Vector
// Adding an three elements to av0
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// After first push_back
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Hello from A Destructor
// After second push_back
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// After third push_back, size av0 is 3
// Reserving 10 elements
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Pushing three more elements
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Accessing elements with the [] operator
// av0[0].x is 2
// av0[1].x is 10
// av0[2].x is 100
// Accessing elements with the front and back
// av0.front().x is 2
// av0.back().x is 103
// Making a copy of av0
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Size of av1 is 6
// av1[0].x is 2
// Shrinking av0 with pop_back
// Hello from A Destructor
// Size of av0 is 5
// Hello from A Destructor
// Size of av0 is 4
// Constructing  AVec_t with 10 elements
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Size of av2 is 10
// av2[0].x is 1
// Clearing av2
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Size of av2 is 0
// push another on av2
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// push another on av2
// Hello from A::A(int) constructor
// Hello from A Copy constructor
// Hello from A Destructor
// Creating A Pointer Vector
// Adding an three elements to apv0
// Hello from A::A(int) constructor
// Hello from A::A(int) constructor
// Hello from A::A(int) constructor
// Size of apv0 is 3
// Clearing apv0
// Size of apv0 is 0
// Main program exiting
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
// Hello from A Destructor
