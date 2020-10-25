#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define LEN 12
#define COUNT 8
 
typedef struct _node //For 
{
    int key;
    struct _node* left;//왼쪽 자식 노드
    struct _node* right;//오른쪽 자식 노드
}node;
 
int name2int(char c)
{
    return c - 'A';
}
int int2name(int i)
{
    return i + 'A';
}
void init_btree(node** base) //tree 초기화 함수 (base 생성)
{
    *base = (node*)malloc(sizeof(node)); //base에 동적할당
    (*base)->left = NULL; //왼쪽 자식 초기화
    (*base)->right = NULL; //오른쪽 자식 초기화
}
 
node* bti_search(int key, node* base, int* level)
//binary tree 검색
{
    node* s;
    s = base->left; // root node로 이동
    (*level) = 1;
    while (s != NULL && key != s->key) //leaf노드까지&&key값이 일치할 때까지
    {
        if (key < s->key)//찾고자하는 값이 현재 노드 값보다 작다면
        {
            s = s->left; //왼쪽으로 이동
        }
        else //크다면
        {
            s = s->right;//오른쪽으로 이동
        }
        (*level)++;//한단계씩 내려갈 때마다 level 증가
    }
    if (s == NULL) //검색 실패
    {
        (*level) = 0;
        return NULL;
    }
    else//검색 성공
    {
        return s; //해당 노드 포인터 반환
    }
}
 
node* bti_insert(int key, node* base, int* num)
// binary tree 삽입
{
    node* p, * s;
    p = base;
    s = base->left;
    while (s != NULL)//트리 끝까지 반복
    {
        p = s;//이전 노드 임시저장
        if (key < s->key)//찾고자하는 값이 현재 노드 값보다 작다면
        {
            s = s->left;//왼쪽으로 이동
        }
        else//크다면
        {
            s = s->right;//오른쪽으로 이동
        }
    }
    s = (node*)malloc(sizeof(node)); //새로운 노드 동적할당
    s->key = key; //key값 저장
    s->left = NULL; //왼쪽 자식 초기화
    s->right = NULL; //오른쪽 자식 초기화
    if (key < p->key || p == base) //상위노드가 base이거나 추가할 노드 값보다 작다면
    {
        p->left = s; //왼쪽 자식으로 연결
    }
    else
    {
        p->right = s; //오른쪽 자식으로 연결
    }
    (*num)++; // tree 크기 증가
    return s;
}
node* bti_delete1(int key, node* base, int* num)
//이진 트리 노드 삭제 함수
{
    node* parent, * son, * del, * nexth;
    parent = base;
    del = base->left;
    while (del != NULL && key != del->key)
        //search 과정과 유사 //삭제할 노드 찾기
    {
        parent = del;
        if (del->key > key)
        {
            del = del->left;
        }
        else
        {
            del = del->right;
        }
    }
    if (del == NULL) //찾는 노드가 없다면
    {
        return NULL; //NULL 반환, 종료
    }
    if (del->left == NULL && del->right == NULL) 
        //Case #1 external (leaf)노드일 때
    {
        son = NULL;
    }
    else if (del->left != NULL && del->right != NULL)
        //Case #3 양쪽 자식 노드가 존재할 때
    {
        nexth = del->right;
        if (nexth->left != NULL)
            //Case #3-2 root일때 
        {
            while (nexth->left->left != NULL)
            //root의 오른쪽 자식노드의 하위 노드에서 root와 가장 가까운 값 (오른쪽 에서 가장 왼쪽 노드)찾는 과정
            {
                nexth = nexth->left; //왼쪽으로 계속 이동
            }
            son = nexth->left; //찾은 노드에서 왼쪽으로 한칸 더 아래 노드를 자식(차기 root)으로 설정
            nexth->left = son->right; //기존 son 자리에서 son 잘라내는 과정
            son->left = del->left; //기존 root의 왼쪽 자식 연결 이어받음
            son->right = del->right;//기존 root의 오른쪽 자식 연결 이어받음
        }
        else
            //Case #3-1  자식노드가 2개일 때(root X)
        {
            son = nexth; //오른쪽 자식 노드(nexth)가 삭제되는 노드 대체
            son->left = del->left; //기존 노드의 왼쪽 자식 연결 이어 받음
        }
    }
    else //Case #2 자식 노드가 1개일 때
    {
        if (del->left != NULL) //왼쪽 자식 노드가 존재할 때
        {
            son = del->left; //기존 노드의 왼쪽 자식 이어 받음
        }
        else //오른쪽 자식 노드가 존재할 때
        {
            son = del->right;  //기존 노드의 오른쪽 자식 이어 받음
        }
    }
    if (key < parent->key || parent == base)
        //삭제할 노드의 상위노드가 base이거나 추가할 노드 값보다 작다면
    {
        parent->left = son;//왼쪽 자식으로 연결
    }
    else
    {
        parent->right = son;//오른쪽 자식으로 연결
    }
    free(del); //노드 삭제
    (*num)--; //트리 크기 감소
    return parent;
}
node* bti_delete(int key, node* base, int* num)
//이진 트리 노드 삭제 함수(향상된 버전)
{
    node* parent, * son, * del, * nexth;
    parent = base;
    del = base->left;
    while (del != NULL && key != del->key)
        //search 과정과 유사 //삭제할 노드 찾기
    {
        parent = del;
        if (del->key > key)
        {
            del = del->left;
        }
        else
        {
            del = del->right;
        }
    }
    if (del == NULL) //찾는 노드가 없다면
    {
        return NULL;  //NULL 반환, 종료
    }
    if (del->right == NULL) 
      //Case #1 오른쪽 자식 노드 없을 때
    {
        son = del->left; //기존 노드의 왼쪽 자식 이어 받음 (NULL이라도 상관 X)
    }
    else if (del->right->left == NULL)
        //Case #2 오른쪽 자식 노드의 왼쪽 자식 노드가 없을 때
    {
        son = del->right; //기존 노드의 오른쪽 자식 이어 받음
        son->left = del->left; //기존 노드의 왼쪽 자식이 이어 받은 자식의 왼쪽 노드가 된다.
    }
    else
        //Case #3 그 이외 (이전 함수의 Case #3과 유사)
    {
        nexth = del->right;
        while (nexth->left->left != NULL)
            //삭제할 노드의 오른쪽 자식노드의 하위 노드에서 삭제할 노드와 가장 가까운 값 (오른쪽 에서 가장 왼쪽 노드)찾는 과정
        {
            nexth = nexth->left;//왼쪽으로 계속 이동
        }
        son = nexth->left; //찾은 노드에서 왼쪽으로 한칸 더 아래 노드를 자식(차기 노드)으로 설정
        nexth->left = son->right; //기존 son 자리에서 son 잘라내는 과정
        son->left = del->left; //기존 노드(삭제 예정)의 왼쪽 자식 연결 이어받음
        son->right = del->right; //기존 노드(삭제 예정)의 오른쪽 자식 연결 이어받음
    }
    if (key < parent->key || parent == base)
        //삭제할 노드의 상위노드가 base이거나 추가할 노드 값보다 작다면
    {
        parent->left = son;//왼쪽 자식으로 연결
    }
    else
    {
        parent->right = son;//오른쪽 자식으로 연결
    }
    free(del); //노드 삭제
    (*num)--; //트리 크기 감소
    return parent;
}
 
void bti_sort(node* p, int* a, int* index)
//in-order traverse 기반 트리 정렬 함수
//in-order traverse 정렬은 노드를 왼쪽->오른쪽 순서대로 level에 관계없이 정렬 가능하다.
{
    if (p != NULL) // 종료 조건
    {
        bti_sort(p->left, a, index); //왼쪽 자식 노드 탐색
        a[(*index)++] = p->key; //정렬배열에 현재 노드 값 저장
        bti_sort(p->right, a, index); //오른쪽 자식 노드 탐색
    }
}
node* _balance(int N, int* a, int* index)
//in-order traverse 기반 tree 생성
//in-order traverse로 이진 트리를 생성하면 균형잡힌 트리를 생성 가능하다.
{
    int nl, nr;
    node* p;
    if (N > 0) //종료 조건 #1 하위 노드가 존재할 경우에만 재귀호출
    {
        nl = (N - 1) / 2; //왼쪽 하위 노드 개수 계산
        nr = N - nl - 1; //오른쪽 하위 노드 개수 계산 //root(중심 부모 노드)는 빼고 계산
        p = (node*)malloc(sizeof(node)); //새 노드 동적할당
        p->left = _balance(nl, a, index); //새 노드의 왼쪽 자식 노드 연결
        p->key = a[(*index)++];//새 노드에 정렬된 배열 값 저장
        p->right = _balance(nr, a, index); //새 노드의 오른쪽 자식 노드 연결
        return p;
    }
    else //종료 조건 #2 leaf 노드의 자식 노드 연결 위해
    {
        return NULL; //자식 노드로 NULL return
    }
}
 
void bti_print_tree_recur(node* root, int space)
//in-order traverse 기반 트리 출력 함수
{
    // Base case 
    if (root == NULL)
        return;
 
    // Increase distance between levels 
    space += COUNT;
 
    // Process right child first 
    bti_print_tree_recur(root->right, space);
 
    // Print current node after space 
    // count 
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%c\n", int2name(root->key));
 
    // Process left child 
    bti_print_tree_recur(root->left, space);
}
 
void bti_print_tree(node* base)
//bti_print_tree_recur wrapper 함수
{
    int i;
    for (i = 0; i < 4; i++)
    {
        printf("----------");
    }
    printf("\n");
    bti_print_tree_recur(base->left, 0); //space는 0으로
    for (i = 0; i < 4; i++)
    {
        printf("----------");
    }
    printf("\n");
}
void bti_delete_all_recur(node* p)
{//post-order traverse 기반 이진트리 동적할당 해제
    if (p != NULL)
    {
        bti_delete_all_recur(p->left);
        bti_delete_all_recur(p->right);
        free(p);
    }
}
void bti_delete_all(node* base)
//bti_delete_all_recur wrapper 함수
{
    bti_delete_all_recur(base->left);
    free(base);//base도 동적할당 해제한다.
}
int main()
{
    node* base, * new_balance, * search_result;
    int i;
    int size = 0, level = 0;
    int search, del;
    int sort_data[LEN];
    char data[LEN] = { 'F','B','A','D','C','O','L','G','M','H','N','K' }; //배열 순서 중요!(특히 root 먼저) //순서에 따라 트리 배열이 달라진다.
    init_btree(&base); // base 포인터 초기화
    for (i = 0; i < LEN; i++)
    {
        bti_insert(name2int(data[i]), base, &size);//배열------->이진트리 삽입
    }
    printf("기존 이진 트리\n");
    bti_print_tree(base); //트리 출력
    i = 0; //index 초기화
 
    bti_sort(base->left, sort_data, &i);//이진트리 ----(정렬)---->배열
    init_btree(&new_balance); // new_balance 포인터 초기화
    i = 0; //index 초기화 
    new_balance->left = _balance(size, sort_data, &i);//정렬된 배열------->새로운 균형된 이진트리
    printf("balanced version\n");
    bti_print_tree(new_balance);//트리 출력
 
    search = name2int('H'); 
    search_result = bti_search(search, new_balance, &level); //H 노드 검색
 
    if (search_result == NULL)
    {
        printf("찾고자하는 노드가 없습니다.\n");
    }
    else
    {
        printf("%c의 검색결과, tree level: %d left child: %c right child: %c\n", int2name(search_result->key), level, (search_result->left == NULL) ? ' ' : int2name(search_result->left->key), (search_result->right == NULL) ? ' ' : int2name(search_result->right->key));
    }
    del = name2int('L');
    if (bti_delete1(del, new_balance, &size) != NULL) //L 노드 삭제
    {
        printf("%c 삭제!\n", int2name(del));
    }
    bti_print_tree(new_balance);//트리 출력
 
    del = name2int('C');
    if (bti_delete(del, new_balance, &size) != NULL) //C 노드 삭제
    {
        printf("%c 삭제!\n", int2name(del));
    }
    bti_print_tree(new_balance);//트리 출력
 
    bti_delete_all(base); 
    bti_delete_all(new_balance); 
}