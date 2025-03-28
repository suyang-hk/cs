#include <iostream>
using namespace std;

// 定义链表节点
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(NULL) {}  // 构造函数
};

// 快慢指针找到链表中点，并拆分链表
ListNode* getMiddle(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* slow = head;
    ListNode* fast = head->next; // 这里 fast 先走一步，保证偶数时 slow 停在前半部分

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    ListNode* mid = slow->next;
    slow->next = NULL;  // 断开链表
    return mid;
}

// 合并两个有序链表
ListNode* merge(ListNode* left, ListNode* right) {
    if (!left) return right;
    if (!right) return left;

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (left && right) {
        if (left->val <= right->val) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = left ? left : right;  // 连接剩余部分
    return dummy.next;
}

// 归并排序主函数
ListNode* mergeSort(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* mid = getMiddle(head);
    ListNode* left = mergeSort(head);
    ListNode* right = mergeSort(mid);

    return merge(left, right);
}

// 打印链表
void printList(ListNode* head) {
    while (head) {
        cout << head->val << " ";
        head = head->next;
    }
    cout << endl;
}

// 测试代码
int main() {
    ListNode* head = new ListNode(4);
    head->next = new ListNode(3);
    head->next->next = new ListNode(1);
    head->next->next->next = new ListNode(2);
    head->next->next->next->next = new ListNode(5);

    cout << "原链表: ";
    printList(head);

    head = mergeSort(head);

    cout << "排序后: ";
    printList(head);

    return 0;
}
