#include <iostream>
#include <algorithm>
using namespace std;
 
template <class T>
class CIntPair
{
public:
    static constexpr bool LEFT = false;
    static constexpr bool RIGHT = true;
    bool direction = LEFT;
    T value = 0;
};
 
template <class T>
class CArrangement
{
private:
    class CRtn
    {
    public:
        bool found;
        CIntPair<T> pair;
        size_t index;
        CRtn(size_t i, CIntPair<T> & p) : pair(p), index(i), found(false) {}
    };
    size_t n;
    unsigned long long counter = 1;
    CIntPair<T> * arr;
    // Function: MaxMovableIndex
    // Description:
    //     A number(CIntPair) is movable, only if the adjacent number that the direction of the movable number pointed is less than
    //     the removable number.
    //     Therefor this procedure is to find the maximum number of all removable numbers.
    //     If there were the maximun number exist, permutation would be able to go on, otherwise there wouldn't be the next permuatation.
    //     And CRtn().found shows whether we could find the max movable number.
    //     Simultaneously, CRtn().index stores the index of removable number, CRtn().pair stores the movable number itself then.
    CRtn MaxMovableIndex()
    {
        size_t i;
        CIntPair<T> t;
        CRtn r(0, t);
        for (i = 0; i < n; ++i)
        {
            if (arr[i].direction == CIntPair<T>::LEFT)
            {
                if (i > 0 && arr[i - 1].value < arr[i].value && r.pair.value < arr[i].value)
                {
                    r.pair = arr[i];
                    r.index = i;
                    r.found = true;
                }
            }
            else
            {
                if (i < n - 1 && arr[i + 1].value < arr[i].value && r.pair.value < arr[i].value)
                {
                    r.pair = arr[i];
                    r.index = i;
                    r.found = true;
                }
            }
        }
        return r;
    }
    // Function: Swap
    // Description: Swap two adjacent numbers by index and direction.
    void Swap(size_t i)
    {
        if (arr[i].direction == CIntPair<T>::LEFT)
            swap(arr[i], arr[i - 1]);
        else
            swap(arr[i], arr[i + 1]);
    }
    // Function: ChangeDirection
    // Description: Change all directions which belong to p, such that p > r.pair.value.
    void ChangeDirection(CRtn & r)
    {
        size_t i;
        for (i = 0; i < n; ++i)
            if (arr[i].value > r.pair.value)
                arr[i].direction = !arr[i].direction;
    }
public:
    CArrangement(size_t k) : n(k)
    {
        size_t i;
        arr = new CIntPair<T>[n];
        for (i = 0; i < n; ++i)
        {
            arr[i].direction = CIntPair<T>::LEFT;
            arr[i].value = i + 1;
        }
    }
    CArrangement(const T & initial, size_t k) : n(k)
    {
        size_t i;
        arr = new CIntPair<T>[n];
        for (i = 0; i < n; ++i)
        {
            arr[i].direction = CIntPair<T>::LEFT;
            arr[i].value = static_cast<T>(i + initial);
        }
    }
    ~CArrangement()
    {
        delete[] arr;
        n = 0;
    }
    bool Permute()
    {
        if (n <= 1)
            return false;
        // 1st. find the maximun movable number in arr list as r.
        CRtn r = MaxMovableIndex();
        if (r.found)
        {
            // 2nd. Swap r with the number adjacent to r.
            Swap(r.index);
            // 3rd. Change all directions of p, such that p > r.pair.value.
            ChangeDirection(r);
            ++counter;
        }
        return r.found;
    }
    template <class TX>
    friend ostream & operator << (ostream & o, const CArrangement<TX> & ra);
};
 
template <class T>
ostream & operator << (ostream & o, const CArrangement<T> & ra)
{
    size_t i;
    o << ra.counter << ":\t";
    for (i = 0; i < ra.n; ++i)
        o << ra.arr[i].value << " ";
    return o;
}
 
int main(void)
{
    CArrangement<int> a(4);
    cout << a << endl;
    while (a.Permute())
        cout << a << endl;
    return 0;
}
