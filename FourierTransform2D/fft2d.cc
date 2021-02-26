// Distributed two-dimensional Discrete FFT transform
// MOUMITA DEY
// ECE8893 Project 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

// declaring functions
void Transform1D(Complex* h, int w, Complex* H);
void Transform1D_Inv(Complex* H, int w, Complex* h);
void Compute1DFFT(int nCPUs, int myRank, int num_rows_per_cpu, Complex* d, int width, int start_row, Complex* Hcompute, int option);
void SentTo0(int nCPUs, int myRank, int width, int num_rows_per_cpu, int start_row, Complex* Hcompute, Complex* dfft);
void MakeTranspose(int height, int width, Complex* orig, Complex* trans);
void SendToAll(int nCPUs, int width, int myRank, int num_rows_per_cpu, Complex* d2send, Complex* hin);

void Transform2D(const char* inputFN) 
{ 
  // Step 1 : Use InputImage object to read in the Tower.txt file and find the width/height of the input image
  InputImage image(inputFN);  // Create the helper object for reading the image
  int height = image.GetHeight();
  int width = image.GetWidth();

  // Step 2 : Use MPI to find how many CPUs in total, and which one this process is = nCPUs, myRank
  int myRank; // current CPU rank
  int nCPUs; // number of CPUs
  MPI_Comm_size(MPI_COMM_WORLD, &nCPUs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  int start_row[nCPUs]; // start row for each rank
  int num_rows_per_cpu = height / nCPUs;
  
  // initializing parameters
  for (int i = 0; i < nCPUs; i++)
    start_row[i] = width / nCPUs * i;

  // Step 3 : Allocate an array of Complex object of sufficient size to hold the 2d DFT results (size is width * height)
  Complex dfft1[height * width];  // stores 1dfft final results; with rank 0
  Complex dfft2[height * width];  // stores 2dfft final results; with rank 0
 
  // Step 4 : Obtain a pointer to the Complex 1d array of input data
  Complex *d = image.GetImageData();
  Complex Hcompute[num_rows_per_cpu * width];   // 1D FFT results local to each rank
  Complex hin[width * num_rows_per_cpu];  // data received from rank 0 pertaining to that particular rank

  // Step 5 : Do the individual 1D transforms on the rows assigned to your CPU
  // Step 6 : Send the resultant transformed values to the appropriate other processors for the next phase.
  // Step 7 : Receive messages from other processes to collect your columns

  // Allocate relevant rows for each rank in hin[]
  for (int i = 0; i < width * num_rows_per_cpu; i++)
  	hin[i] = d[start_row[myRank] * width + i];

  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  Compute1DFFT(nCPUs, myRank, num_rows_per_cpu, hin, width, start_row[myRank], Hcompute, 0);

  // Now all ranks will send its computed Hcompute to rank 0 and rank 0 will store in dfft1
  SentTo0(nCPUs, myRank, width, num_rows_per_cpu, start_row[myRank], Hcompute, dfft1);
    
  // Save to file  
  if (myRank == 0)
    image.SaveImageData("MyAfter1D.txt", dfft1, width, height);

  // Step 8 : When all columns received, do the 1D transforms on the columns
  Complex dfft1_tr[width * height]; // transpose of dfft1
  Complex dfft2_tr[width * height]; // transpose of dfft2

  // Create transpose of dfft1
  if (myRank == 0)
	MakeTranspose(height, width, dfft1, dfft1_tr);

  // Now send relevant rows to all CPUs from 0
  SendToAll(nCPUs, width, myRank, num_rows_per_cpu, dfft1_tr, hin);

  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  Compute1DFFT(nCPUs, myRank, num_rows_per_cpu, hin, width, start_row[myRank], Hcompute, 0);

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in dfft2_tr
  SentTo0(nCPUs, myRank, width, num_rows_per_cpu, start_row[myRank], Hcompute, dfft2_tr);

  // Create transpose of dfft2_tr
  if (myRank == 0)
	MakeTranspose(height, width, dfft2_tr, dfft2);
    
  // Save to file  
  if (myRank == 0)
    image.SaveImageData("MyAfter2D.txt", dfft2, width, height);
  
  //////////////////////// Computation of 2D FFT Inverse /////////////////////////////////////////////////

  Complex idfft1[width * height];
  Complex idfft1_tr[width * height];
  Complex idfft2_tr[width * height];
  Complex idfft2[width * height];

  // Rank 0 has dfft2 array. It will first send out relevant rows to all other ranks
  SendToAll(nCPUs, width, myRank, num_rows_per_cpu, dfft2, hin);

  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  Compute1DFFT(nCPUs, myRank, num_rows_per_cpu, hin, width, start_row[myRank], Hcompute, 1);

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in idfft1
  SentTo0(nCPUs, myRank, width, num_rows_per_cpu, start_row[myRank], Hcompute, idfft1);

  // Create transpose of idfft1
  if (myRank == 0)
	MakeTranspose(height, width, idfft1, idfft1_tr);

  // Rank 0 has idfft1_tr array. It will first send out relevant rows to all other ranks for 2D FFT operation
  SendToAll(nCPUs, width, myRank, num_rows_per_cpu, idfft1_tr, hin);

  // Now all ranks will calculate inverse 1D FFT on their specific set of rows
  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  Compute1DFFT(nCPUs, myRank, num_rows_per_cpu, hin, width, start_row[myRank], Hcompute, 1);

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in idfft2_tr
  SentTo0(nCPUs, myRank, width, num_rows_per_cpu, start_row[myRank], Hcompute, idfft2_tr);

  // Create transpose of idfft2_tr
  if (myRank == 0)
	MakeTranspose(height, width, idfft2_tr, idfft2);

  // Save to file  
  if (myRank == 0)
    image.SaveImageData("MyAfterInverse.txt", idfft2, width, height);
  
  // cout<<"Finalize "<<myRank <<endl;
  MPI_Finalize();
}

void Transform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  int N = w;    // assigning this just for simplicity purpose
  Complex omega = 0;
  Complex temp1, temp2;

  for(int j = 0; j < N; j++)
  {
    temp1 = 0;
    temp2 = 0;
    for(int i = 0; i < N; i++)
    {
      omega = Complex(cos(2*M_PI*j*i/N), -sin(2*M_PI*j*i/N));
      temp2 = omega.operator*(h[i]);
      temp1 = temp1.operator+(temp2);
    }
    H[j] = temp1; 
  }
}

void Transform1D_Inv(Complex* H, int w, Complex* h)
{
  int N = w;    // assigning this just for simplicity purpose
  Complex omega;
  Complex temp1, temp2;

  for(int j = 0; j < N; j++)
  {
    temp1 = 0;
    temp2 = 0;
    for(int i = 0; i < N; i++)
    {
      omega = Complex(cos(2*M_PI*j*i/N), sin(2*M_PI*j*i/N));
      temp2 = omega.operator*(H[i]);
      temp1 = temp1.operator+(temp2);
    }
    h[j] = Complex(temp1.real/N, temp1.imag/N); 
  }
}

void Compute1DFFT(int nCPUs, int myRank, int num_rows_per_cpu, Complex* d, int width, int start_row, Complex* Hcompute, int option)
{
  	Complex d2give[width];  // part of matrix to be given to Transform1D()
	Complex H[width]; // output of Transform1D
	int iter[nCPUs];
	int num_completed_rows[nCPUs];  // number of computed rows of each rank
	for (int i = 0; i < nCPUs; i++)
		num_completed_rows[i] = 0;

	for(iter[myRank] = 0; iter[myRank] < num_rows_per_cpu; iter[myRank]++)
	{
	  for(int i = 0; i < width; i++)
	    d2give[i] = d[num_completed_rows[myRank] * width + i];
	  if (option == 0)
		Transform1D(d2give, width, H);  
	  else
	  	Transform1D_Inv(d2give, width, H);
	  for(int i = 0; i < width; i++)
	    Hcompute[num_completed_rows[myRank] * width + i] = H[i]; 
	  num_completed_rows[myRank]++;
	}
}

void SentTo0(int nCPUs, int myRank, int width, int num_rows_per_cpu, int start_row, Complex* Hcompute, Complex* dfft)
{
	int rc;
	Complex Hrec[width * num_rows_per_cpu];

	if (myRank == 0)
	{
		// store computed values of rank 0 directly to the matrix
		for (int i = 0; i < width * num_rows_per_cpu; i++)
		  dfft[start_row * width + i] = Hcompute[i];
		for (int j = 1; j < nCPUs; j++)
		{
		  MPI_Status status;
		  //rc = MPI_Recv(Hrec, sizeof(Hrec), MPI_COMPLEX, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		  rc = MPI_Recv(Hrec, num_rows_per_cpu*width*sizeof(Complex), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		  if (rc != MPI_SUCCESS)
		  {
		    cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
		    MPI_Finalize();
		  }
		  // now store the received results in the dfft1[]
		  for (int i = 0; i < width * num_rows_per_cpu; i++)
		    dfft[width / nCPUs * status.MPI_SOURCE * width + i] = Hrec[i]; 
		}
	}
	else
	{
		rc = MPI_Send(Hcompute, width*num_rows_per_cpu*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		if (rc != MPI_SUCCESS)
		{
		  cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
		  MPI_Finalize();
		} 
	}     
}

void MakeTranspose(int height, int width, Complex* orig, Complex* trans)
{
    for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
        trans[i * width + j] = orig[j * height + i];
    }      
}

void SendToAll(int nCPUs, int width, int myRank, int num_rows_per_cpu, Complex* d2send, Complex* hin)
{
  int rc;

  if (myRank == 0)
  {
    Complex H2send[width * num_rows_per_cpu];
    for (int k = 1; k < nCPUs; k++)
    {
      for (int j = 0; j < width * num_rows_per_cpu; j++)
        H2send[j] = d2send[width / nCPUs * k * width + j];  
      rc = MPI_Send(H2send, width*num_rows_per_cpu*sizeof(Complex), MPI_CHAR, k , 0, MPI_COMM_WORLD);
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
        MPI_Finalize();
      } 
    }
    // storing only for rank 0
    for (int i = 0; i < width * num_rows_per_cpu; i++)
      hin[i] = d2send[i];
  }
  else
  {
    MPI_Status status;
    rc = MPI_Recv(hin, width*num_rows_per_cpu*sizeof(Complex), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
      MPI_Finalize();
    }
  }
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  
  // MPI initialization here
  int rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) 
  {
    cout<<"Error starting MPI Program. Terminating.\n";
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  // Perform the transform.
  Transform2D(fn.c_str()); 
}  
