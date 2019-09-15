#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#define SS_INF -100
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Solves a square equation ax2 + bx + c = 0
//!
//! @param [in] a a ‐coefficient
//! @param [in] b b‐coefficient
//! @param [in] c c ‐coefficient
//! @param [out] x1 Pointer to the 1st root
//! @param [out] x2 Pointer to the 2nd root
//!
//! @return Number of roots
//!
//! @note In case of infinite number of roots,
//! returns SS_INF_ROOTS.
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐



int SolveLinear(double a, double b, double *x1)
{
    if (a == 0) {
        if (b==0) return SS_INF;
        return 0;
    }
    if (b == 0) {
        *x1 = 0;
        return 1;
    }
    *x1 = -b / a;
    return 1;
}


int SolveSquare(double a, double b, double c, double *x1, double *x2)
{
    assert (isfinite (a));
    assert (isfinite (b));
    assert (isfinite (c));
    assert (x1 != NULL);
    assert (x2 != NULL);
    assert (x1 != x2);
    if (a == 0) return SolveLinear(b,c,x1);
    if (c ==0){
        *x2 = 0;
        return SolveLinear(a,b,x1)+1;

    }
    if (b==0) {
        if (c < 0){
            *x1 = sqrt(-c);
            *x2 = -sqrt(-c);
            return 2;
        }
        return 0;
    }
    double d = b*b - 4*a*c;
    if (d < 0) return 0;
    if (d == 0){
        *x1= -b / 2 / a;
                return 1;
    }
    *x1 = (-b - sqrt(d))/2/a;
    *x2 = (-b + sqrt(d))/2/a;
    return 2;
}



void print(int nRoot, double x1, double x2)
{
    switch ( nRoot ) {
        case 2: printf(" x = %lg ", x2);
        case 1: printf(" x = %lg ", x1) ;
            break;
        case SS_INF : printf("infinity solutions");
            break;
        case 0: printf("no solutions");
            break;
        default: printf ("main(): ERROR: nRoots = %d\n", nRoot);
            break;

    }
}

double*  InputNumberSt(const char promt[], int amount )
{
    static double data[100] = {0};
    char c;
    int k=0;
    while ((c = *(promt+k++)) != '\0')
        putchar(c);
    for (int i = 0; i < amount; i++)
    {
        if (!scanf("%lg", data+i))
        {
            fflush(stdin);
            printf("input incorrect try better :\n");
            i--;
        }
    }
    return data;
}

double* InputNumberAuto( int amount )
{
   double *data = (double*)calloc(amount, sizeof(data[0]));

    for (int i = 0; i < amount; i++)
    {
        *(data+i) = rand() % 1000;

    }
    return data;

}

char Compare(int nRoot, double x1, double x2, double a, double b, double c)
{
    double e = 0.01;
    char ch = 1;
    switch ( nRoot ) {
        case 2: if (fabs(a*x2*x2+b*x2+c) > e)
                    ch = 0;
        case 1:if (fabs(a*x1*x1+b*x1+c) > e)
                ch = 0;
            break;
        case SS_INF : if (!((a==0) && (b == 0) && (c == 0)))
                ch=0;
            break;
        case 0: if (!isnan(x1) || !isnan(x2))
            ch = 0;
            break;
        default: printf ("main(): ERROR: nRoots = %d\n", nRoot);
        ch = 0;
            break;
    }
    return ch;
}


char Utest(void)
{
    double a=NAN,b=NAN,c=NAN;
    double x1=NAN,x2=NAN;
    int amount = 3 ;
    for (int i =0; i<100; i++) {
        double *coeff = InputNumberAuto(amount);
        a= 500; //*coeff;
        b =  169.0000000000000000000000000000000001; //*(coeff+1);
        c = 724; //*(coeff+2);
        int nRoot = SolveSquare(a, b, c, &x1, &x2);
        if (!Compare(nRoot, x1, x2, a, b, c)) {
            printf(" nRoot %d Coeffs were %lg %lf %lg roots: %lg %lg error", nRoot, a,b,c,x1,x2);
            return 1;
        }
        free(coeff);
    }
    return 0;
}


int main()
{
    printf("program solves your quadratic \n");
    printf("Ohh, forget about tests.. wait.. \n");
    if (Utest()) {
        return 1;
    }
    printf("ok its works \n");
    double x1=NAN,x2=NAN;
    int amount = 3 ;
    double *coeff= InputNumberSt("input a b c : \n", amount);
    int nRoot = SolveSquare(*coeff,coeff[1],*(coeff+2),&x1,&x2);
    print(nRoot,x1,x2); // if its only one root its always x1
    return 0;
}
