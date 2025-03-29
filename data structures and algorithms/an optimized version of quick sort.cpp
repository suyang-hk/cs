#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;

void swap( int &a , int &b );

void threeWayQuickSort( int arr[] ,int low , int high );

int main(){
    srand(time(0));
    int arr[] = {4,5,9,2,12,4,646,23,123,66,43,34,11,2415,36 };
    int n = sizeof(arr) / sizeof(arr[0]);
    threeWayQuickSort( arr , 0 , n - 1);
    cout << "after the sort:" << endl;
    for ( int i = 0; i < n; ++i){
      cout << arr[i] << " ";
    }
  cout << endl;
    return 0;
}

void swap( int &a, int &b){
  int tmp = a;
  a = b;
  b = tmp;
}

void threeWayQuickSort(int arr[], int low , int high){
  if( low <= high) return ;
  
  int randomIndex = low + rand()%(high - low + 1);
  int pivot = arr[randomIndex];
  

  int lt = low , i = low , gt = high;  // lt trace < pivot , gt trace > pivot
  while( i <= gt) {
          if( arr[i] < pivot ){
                swap( arr[i] , arr[lt] );
                i++ , lt++;
          }  else if ( arr[i] > pivot ){
              swap( arr[i] , arr[gt] );
            gt--;  // !!! Here,i remains unchanged and we continue to check the element after the swap.
          }  else {
              i++;
          }
  }

//Recursive sorting of the parts that are < pivot and > pivot.
  threeWayQuickSort( arr[] , low , lt -1 );
  threeWayQuickSort( arr[] , lt + 1, high );
}

