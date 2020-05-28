#include <stdio.h>
#include <math.h>

/// Calculate the log to base r
float logn(int n,int r)
{
    return log(n) / log(r);
}

/// Get the nearest 2^n number with minmum value = 4
int Get_nearest_2power_num (int input_value){
    float x;
    int ceiled_x;
    if (input_value >=0 && input_value<=4)
        return 4;
    else {
        x=logn(input_value,2);
        ceiled_x=(int)ceil(x);
        return (int)pow(2,ceiled_x);
    }
}



/// Check if the number is 2^ where n is integer or not.
int Check_if_num_isbinay (int input_value) {

    float x;
    x=logn(input_value,2);
    float remainder;
    remainder=x-(floor(x));
    if (remainder==0)
        return 1;
    else
        return 0;
}
