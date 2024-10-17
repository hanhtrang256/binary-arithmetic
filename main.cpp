#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
using namespace std;

/* INPUT, OUTPUT, OTHER FUNCTIONS */
char toNum8bit(char bit[]) {
    char ret = 0;
    for (int i = 0; i < 8; ++i) 
        if ((int)bit[i] > 0) 
            ret |= (1 << i);
    return ret;
}

short int toNum16bit(char bit[]) {
    short ret = 0;
    for (int i = 0; i < 16; ++i) 
        if ((int)bit[i] > 0)
            ret |= (1 << i);
    return ret;
}

void resetArr(char a[], int len) {
    for (int i = 0; i < len; ++i) a[i] = 0;
    a[len] = '\0';
}

void input(char &A, char letter) {
    int x;
    while (true) {
        cout << "Input " << letter << ": ";
        cin >> x;
        if (x < -128 || x > 127) cout << "Out of 8 bits signed integers range!!!\n";
        else {
            A = x;
            break;
        }
    }
}

void output(char a[], int len, bool eight, string str = "") {
    if (str.empty() == false) cout << str;
    for (int i = len - 1; i >= 0; --i) cout << (int)a[i];
    cout << " (2's complement: " << (eight ? (int)toNum8bit(a) : toNum16bit(a)) << ")" << '\n';
}

void shiftRight(char a[], int len) {
    for (int i = 0; i < len - 1; ++i) 
        a[i] = a[i + 1];
    a[len - 1] = 0;
}

void shiftLeft(char a[], int len) {
    for (int i = len - 1; i > 0; --i) 
        a[i] = a[i - 1];
    a[0] = 0;
}

void createArrBitOne(char ret[], int len) {
    resetArr(ret, len);
    ret[0] = 1;
}

void copyArr(char a[], char b[], int len) {
    for (int i = 0; i < len; ++i) a[i] = b[i];
    a[len] = '\0';
}

int compare(char a[], char b[], int len = 8) {
    // -1 if a < b; 0 if a = b; 1 if a > b
    for (int i = len - 1; i >= 0; --i) {
        if ((int)a[i] ^ (int)b[i] > 0) {
            if ((int)a[i] < (int)b[i]) return -1;
            return 1;
        }
    }
    return 0;
}

/* ADD, SUBTRACT, MULTIPLY, DIVIDE */
/* Prototype: func_name(bitA, bitB, res, len) */

void add(char bitA[], char bitB[], char res[], int len) {
    bool carry = false;
    resetArr(res, len);
    for (int i = 0; i < len; ++i) {
        res[i] |= ((int)bitA[i] ^ (int)bitB[i] ^ (int)carry);
        bool nextCarry = ((int)bitA[i] > 0 && (int)bitB[i] > 0) || (carry == 1 && ((int)bitA[i] ^ (int)bitB[i]) == 1);
        carry = nextCarry;
    }
}

void get2Comple(char a[], char ret[], int len) {
    char oneComple[len + 1];
    oneComple[len] = '\0';
    for (int i = 0; i < len; ++i) oneComple[i] = a[i] ^ 1;

    char bit1[len + 1];
    createArrBitOne(bit1, len);

    add(oneComple, bit1, ret, len);
}

void sub(char bitA[], char bitB[], char res[], int len) {
    /* Sub rules */
    // 0 - 0 = 0, 1 - 0 = 1, 1 - 1 = 0, 0 - 1 = 1 (borrow 1)
    char twoCompleB[len + 1];
    get2Comple(bitB, twoCompleB, len);
    //output(twoCompleB, len, 1, "2's complement of B = ");
    add(bitA, twoCompleB, res, len);
}

void mul(char bitA[], char bitB[], char res[], int len) {
    // convert A and B to positive 
    char mulA[len + 1];
    resetArr(mulA, len);
    if ((int)bitA[len - 1] == 0) copyArr(mulA, bitA, 8);
    else get2Comple(bitA, mulA, len);

    char mulB[len + 1];
    resetArr(mulB, len);
    if ((int)bitB[len - 1] == 0) copyArr(mulB, bitB, 8);
    else get2Comple(bitB, mulB, len);

    cout << "Convert A to positive: ";
    for (int i = len - 1; i >= 0; --i) cout << (int)mulA[i];
    cout << '\n';

    cout << "Convert B to positive: ";
    for (int i = len - 1; i >= 0; --i) cout << (int)mulB[i];
    cout << '\n';

    // multiply process
    char mulRes[len * 2 + 2];
    resetArr(mulRes, len * 2 + 1);
    for (int i = 0; i < len; ++i) mulRes[i] = mulB[i];

    for (int i = 0; i < len; ++i) {
        if ((int)mulB[i] == 1) {
            bool carry = false;
            for (int j = len; j < len * 2; ++j) {
                char tmp = mulRes[j];
                mulRes[j] = ((int)mulRes[j] ^ mulA[j - len] ^ carry);
                bool nextCarry = ((int)mulA[j - len] > 0 && (int)tmp > 0) || (carry == 1 && ((int)mulA[j - len] ^ (int)tmp) == 1);
                carry = nextCarry;
            }
            mulRes[len * 2] ^= carry;
        }
        shiftRight(mulRes, len * 2 + 1);
    }

    resetArr(res, len * 2);
    // check sign before print results
    if (((int)bitA[len - 1] ^ (int)bitB[len - 1]) > 0) get2Comple(mulRes, res, len * 2);
    else {
        for (int i = 0; i < len * 2; ++i) 
            res[i] = mulRes[i];
    }
}

void divi(char dividend[], char divisor[], char quotient[], char remainder[], int len) {
    /* Simple approach */
    /*  1) Keep shifting left (* 2) the divisor till it's larger than dividend
        2) Then calculate remainder = dividend subtract the divisor (shift right divisor one before)
        3) Repeat the same process if the remainder is still >= divisor
        Note that when shift left divisor ends at time i-th <=> turns bit i-th of quotient on */

    // convert to positive
    char convertDividend[len + 1];
    resetArr(convertDividend, len);
    if ((int)dividend[len - 1] == 0) copyArr(convertDividend, dividend, len);
    else get2Comple(dividend, convertDividend, len);

    char convertDivisor[len + 1];
    resetArr(convertDivisor, len);
    if ((int)divisor[len - 1] == 0) copyArr(convertDivisor, divisor, len);
    else get2Comple(divisor, convertDivisor, len);
    
    resetArr(quotient, len);
    resetArr(remainder, len);
    if (compare(convertDividend, convertDivisor) == -1) { // dividend < divisor
        copyArr(remainder, dividend, len);
        return;
    }

    int maxPossibleShift = 0; 
    for (int i = len - 1; i >= 0; --i) {
        if ((int)convertDivisor[i] == 1) break;
        ++maxPossibleShift;
    }

    // divide process

    char dynamicDividend[len + 1];
    copyArr(dynamicDividend, convertDividend, len);

    while (true) {  // process loop
        if (compare(dynamicDividend, convertDivisor) == -1) {
            copyArr(remainder, dynamicDividend, len);
            break;
        }
        int numShiftLeft = 0;
        char tmpDivisor[len + 1]; 
        copyArr(tmpDivisor, convertDivisor, len);
        while (numShiftLeft < maxPossibleShift && compare(tmpDivisor, dynamicDividend) < 1) { // shift left loop
            shiftLeft(tmpDivisor, len);
            ++numShiftLeft;
        } 
        if (compare(tmpDivisor, dynamicDividend) == 1) {
            shiftRight(tmpDivisor, len);
            --numShiftLeft;
        }

        quotient[numShiftLeft] = 1;

        char tmpRemainder[len + 1];
        sub(dynamicDividend, tmpDivisor, tmpRemainder, len);
        copyArr(dynamicDividend, tmpRemainder, len);
    }

    // dividend and divisor different sign
    if ((int)dividend[len - 1] ^ (int)divisor[len - 1] > 0) {
        char tmp[len + 1];
        get2Comple(quotient, tmp, len);
        copyArr(quotient, tmp, len);
        if ((int)dividend[len - 1] == 1) {
            get2Comple(remainder, tmp, len);
            copyArr(remainder, tmp, len);
        }
    }
    else if ((int)dividend[len - 1] & (int)divisor[len - 1] == 1) {
        char tmp[len + 1];
        get2Comple(remainder, tmp, len);
        copyArr(remainder, tmp, len);
    }
}

// TODO: Remember to put \0 at end of char[]

int main() {
    cout << "[INPUT SECTION]\n";
    char A, B;
    input(A, 'A');
    input(B, 'B');
    cout << "2 input numbers are " << (int)A << " and " << (int)B << '\n' << '\n';

    cout << "[GET BITS OF A & B]\n";
    char bitA[9], bitB[9];
    resetArr(bitA, 8);
    resetArr(bitB, 8);
    for (int i = 7; i >= 0; --i) {
        if (((int)A >> i) & 1) bitA[i] = 1;
        if (((int)B >> i) & 1) bitB[i] = 1;
    }
    output(bitA, 8, 1, "Bits of A = ");
    output(bitB, 8, 1, "Bits of B = ");
    cout << '\n';

    cout << "[ADDITION OF A & B IN BITS]\n";
    char resAdd[9];
    add(bitA, bitB, resAdd, 8);
    output(resAdd, 8, 1, "bit(A) + bit(B) = ");
    cout << '\n';

    cout << "[SUBTRACTION OF A & B IN BITS]\n";
    char resSub[9];
    sub(bitA, bitB, resSub, 8);
    output(resSub, 8, 1, "bit(A) - bit(B) = ");
    cout << '\n';

    cout << "[MULTIPLICATION OF A & B IN BITS]\n";
    char resMul[17]; // 8 bit * 8 bit = 16 bit
    mul(bitA, bitB, resMul, 8);
    output(resMul, 16, 0, "bitA * bit(B) = ");
    cout << '\n';

    cout << "[DIVISION OF A & B IN BITS]\n";
    char quotient[9], remainder[9];
    // handle divide by 0 case
    bool allZero = true;
    for (int i = 0; i < 8; ++i) allZero &= ((int)bitB[i] == 0);
    if (allZero) cout << "Cannot perform divide by zero!\n";
    else {
        divi(bitA, bitB, quotient, remainder, 8);
        output(quotient, 8, 1, "Quotient of A / B = ");
        output(remainder, 8, 1, "Remainder of A / B (A % B) = ");
    }
    return 0;
}
