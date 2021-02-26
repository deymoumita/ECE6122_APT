// Threaded two-dimensional Discrete FFT transform
// MOUMITA DEY
// ECE8893 Project 2

#include <iostream>
#include <string>
#include <math.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

// All global variables
int nThreads = 16;     // total number of threads to be created
int N = 1024;           // number of points in 1D transform = width of image
Complex *d;             // pointer to input and output data 
Complex *W = new Complex[N/2]; // weight array
Complex *temp = new Complex[N*N]; // temp variable for storing transpose
pthread_mutex_t countMutex;  
int count;	// number of threads presently in the barrier
bool *localsense;	// local variable for barrier - each thread has its own copy
bool globalsense;	// global variable for barrier
pthread_mutex_t exitMutex;	// mutex to communicate to main thread
pthread_cond_t exitCond;	// condition to communicate to main thread
bool flag = false;  // to switch between 2D and 2D Inverse

// functions declarations
unsigned ReverseBits(unsigned v);
void compute_weight();
void compute_Transpose();
void thread_create();
void MyBarrier_Init();
void MyBarrier(int myId);
int FetchAndDecrementCount();
void Transform1D(int rowsPerThread, int startingRow);
void* Transform2DTHread(void* v);
void Transform2D(const char* inputFN);

// Function to reverse bits in an unsigned integer
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// Function to precompute weight matrix for N point; only W/2 points needed, remaining are -W
void compute_weight()
{
  for (int i = 0; i < N / 2; i++)
  {
    W[i].real = (cos(2*i*M_PI/N));	// compute weights for forward FFT
    W[i].imag = -(sin(2*i*M_PI/N));
  }
  if (flag == true)	// computing conjugates for inverse FFT
  {
    for (int i = 0; i < N / 2; i++)
      W[i] = W[i].Conj();
  }
}

// Function to compute the Transpose
void compute_Transpose()
{ // compute transpose
  for (int i =  0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
      temp[i*N+j] = d[j*N+i];
  }
  for(int i = 0; i < N*N; i++)
    d[i] = temp[i];  
}

// function to create N threads
void thread_create()
{
  for(int i = 0; i < nThreads; i++)
  {
    pthread_t pt;
    pthread_create(&pt, 0, Transform2DTHread, (void*)i);
  }
}

// Initializing barrier 
void MyBarrier_Init()
{
	pthread_mutex_init(&countMutex, 0);
	localsense = new bool[nThreads];
	for (int i = 0; i < nThreads; i++)
	{
		localsense[i] = true;
	}
	globalsense = true;
	count = nThreads;
	pthread_mutex_init(&exitMutex, 0);
	pthread_cond_init(&exitCond, 0);
	pthread_mutex_lock(&exitMutex);
}

// Barrier function
void MyBarrier(int myId) 
{
	localsense[myId] = ! localsense[myId];
	if (FetchAndDecrementCount() == 1)
	{
		count = nThreads;
		globalsense = localsense[myId];
	}
	else
	{
		while (globalsense != localsense[myId]) {}
	}
}

int FetchAndDecrementCount()
{
	pthread_mutex_lock(&countMutex);
	int myCount = count;
	count--;
	pthread_mutex_unlock(&countMutex);
	return myCount;
}

// Function to calculate 1D DFT as per Danielson–Lanczos Lemma method                    
void Transform1D(int rowsPerThread, int startingRow)
{
  Complex H[N];   //output array
  Complex temp1;  

  for(int l = startingRow; l < startingRow + rowsPerThread; l++)
  {
    for (int i = 0; i < N; i++)
      H[i] = d[l * N + ReverseBits(i)];
    for (int i = 2; i <= N; i += i) 
    {
      for (int j = 0; j < N/i; j++)
      {    
        for (int k = j * i; k < j * i + i/2; k++)
        {
          temp1 = H[k];
          H[k] = H[k] + W[(k - j * i) * N / i] * H[k + i/2];
          H[k + i/2] = temp1 - W[(k - j * i) * N / i] * H[k + i/2];
        }
      }
    }
    for(int i = 0; i < N; i++)
    {
      if (flag == true)
        H[i] = H[i] / N;
      d[l * N + i] = H[i];
    } 
  }
}

// Initial starting function for all threads
void* Transform2DTHread(void* v)
{ 
	unsigned long myId = (unsigned long)v;
	int rowsPerThread = N / nThreads;
	int startingRow = myId * rowsPerThread;
	Transform1D(rowsPerThread, startingRow);
	MyBarrier(myId);
	if (myId == 2)
	  compute_Transpose();
	MyBarrier(myId);
	Transform1D(rowsPerThread, startingRow);
	MyBarrier(myId);
	if (myId == 2)
	  compute_Transpose();
	MyBarrier(myId);
	pthread_mutex_lock(&exitMutex);
	pthread_cond_signal(&exitCond);
	pthread_mutex_unlock(&exitMutex);
	return 0;
}

// The "main" function
void Transform2D(const char* inputFN) 
{ 
  InputImage image(inputFN);  // Create the helper object for reading the image
  if (flag == false)
    d = image.GetImageData();   // Create the global pointer to the image array data    
  N = image.GetHeight(); // height = width of image; assume width=height
  compute_weight();
  thread_create();
  pthread_cond_wait(&exitCond, &exitMutex);
  if (flag == false)
    image.SaveImageData("MyAfter2D.txt", d, N, N);      
  else
    image.SaveImageData("MyAfterInverse.txt", d, N, N);      
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  MyBarrier_Init();     // Initializing the barrier
  flag = false;
  Transform2D(fn.c_str()); // Perform 2D transform
  MyBarrier_Init();
  flag = true;
  Transform2D(fn.c_str()); // Perform 2D Inverse transform
  delete[] temp;
}  
