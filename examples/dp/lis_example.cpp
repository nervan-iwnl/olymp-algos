#include <iostream>
#include <vector>

#include "dp/lis.hpp"

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Формат ввода:
    // n
    // далее n чисел
    //
    // Пример:
    // 8
    // 10 9 2 5 3 7 101 18
    //
    // Вывод:
    // lis length: 4

    int n;
    if (!(cin >> n)) {
        return 0;
    }

    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    int ans = lis(nums);

    cout << "lis length: " << ans << "\n";

    return 0;
}