#include <iostream>
using namespace std;

// 分区函数：返回基准元素的正确位置
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // 选择最后一个元素作为基准
    int i = low - 1; // i 用于记录小于 pivot 的区域

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) { // 找到小于 pivot 的元素
            i++;
            swap(arr[i], arr[j]); // 交换到左边
        }
    }
    swap(arr[i + 1], arr[high]); // 交换基准元素到正确位置
    return i + 1;
}

// 快速排序递归函数
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pivotIndex = partition(arr, low, high); // 获取基准位置
        quickSort(arr, low, pivotIndex - 1); // 递归排序左部分
        quickSort(arr, pivotIndex + 1, high); // 递归排序右部分
    }
}

// 测试函数
int main() {
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);

    quickSort(arr, 0, n - 1);

    cout << "排序后的数组: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}


//-----------------------------------------------------------------------------
//   代码解析
// partition()（分区函数）

// 选择最后一个元素作为基准（pivot）。

// 通过遍历数组，将比 pivot 小的元素放在左边，比 pivot 大的放在右边。

// 返回基准元素的最终位置。

// quickSort()（递归排序）

// 先用 partition() 找到基准位置。

// 递归对左子数组 和 右子数组 进行快速排序。

// 时间复杂度

// 平均情况：O(n log n)

// 最坏情况（已经有序时，每次选择最小或最大元素为 pivot）：O(n²)

// 最佳情况：O(n log n)

// 优化
// 随机选择基准（Randomized Quick Sort）：

// 选择随机元素作为 pivot，避免最坏情况。

// 三路快排（Three-way Quick Sort）：

// 处理大量重复元素时，分成 < pivot、= pivot 和 > pivot 三部分，提高效率。
