#include "BTS_library.h"
void init_tree(node** p)//tree 초기화 함수 (base 생성)
{
	*p = (node*)malloc(sizeof(node)); //base에 동적할당
	(*p)->left =NULL; //왼쪽 자식 초기화
	(*p)->right =NULL; //오른쪽 자식 초기화
}
void* btv_search(void* key, node* base, int* num, int width, FCMP fcmp)
{
	node* s;
	s = base->left; // root node로 이동
	while (s !=NULL && fcmp(key, s +1) !=0) //leaf노드까지&&key값이 일치할 때까지
	{
		if (fcmp(key, s +1) <0)//찾고자하는 값이 현재 노드 값보다 작다면
		{
			s = s->left; //왼쪽으로 이동
		}
		else //크다면
		{
			s = s->right;//오른쪽으로 이동
		}
	}
	if (s ==NULL) //검색 실패
	{
		return NULL;
	}
	else//검색 성공
	{
		return s +1; //해당 노드 포인터 반환
	}
}
void* btv_insert(void* key, node* base, int* num, int width, FCMP fcmp)
{
	node* p, * s;
	p = base;
	s = base->left;
	while (s !=NULL)//트리 끝까지 반복
	{
		p = s;//이전 노드 임시저장
		if (fcmp(key, s +1) <0)//찾고자하는 값이 현재 노드 값보다 작다면
		{
			s = s->left;//왼쪽으로 이동
		}
		else//크다면
		{
			s = s->right;//오른쪽으로 이동
		}
	}
	s = (node*)malloc(sizeof(node) + width); //새로운 노드 동적할당
	memcpy(s +1, key, width);
	s->left =NULL; //왼쪽 자식 초기화
	s->right =NULL; //오른쪽 자식 초기화
	if (fcmp(key, p +1) <0 || p == base) //상위노드가 base이거나 추가할 노드 값보다 작다면
	{
		p->left = s; //왼쪽 자식으로 연결
	}
	else
	{
		p->right = s; //오른쪽 자식으로 연결
	}
	(*num)++; // tree 크기 증가
	return s +1;
}
node* btv_delete1(void* key, node* base, int* num, int width, FCMP fcmp)
{
	node* parent, * son, * del, * nexth;
	parent = base;
	del = base->left;
	while (del !=NULL && fcmp(key, del +1) !=0)
		//search 과정과 유사 //삭제할 노드 찾기
	{
		parent = del;
		if (fcmp(del +1, key) >0)
		{
			del = del->left;
		}
		else
		{
			del = del->right;
		}
	}
	if (del ==NULL) //찾는 노드가 없다면
	{
		return NULL; //NULL 반환, 종료
	}
	if (del->left ==NULL && del->right ==NULL)
		//Case #1 external (leaf)노드일 때
	{
		son =NULL;
	}
	else if (del->left !=NULL && del->right !=NULL)
		//Case #3 양쪽 자식 노드가 존재할 때
	{
		nexth = del->right;
		if (nexth->left !=NULL)
			//Case #3-2 root일때
		{
			while (nexth->left->left !=NULL)
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
		if (del->left !=NULL) //왼쪽 자식 노드가 존재할 때
		{
			son = del->left; //기존 노드의 왼쪽 자식 이어 받음
		}
		else //오른쪽 자식 노드가 존재할 때
		{
			son = del->right;  //기존 노드의 오른쪽 자식 이어 받음
		}
	}
	if (fcmp(key, parent +1) <0 || parent == base)
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
node* btv_delete(void* key, node* base, int* num, int width, FCMP fcmp)
{
	node* parent, * son, * del, * nexth;
	parent = base;
	del = base->left;
	while (del !=NULL && fcmp(key, del +1) !=0)
		//search 과정과 유사 //삭제할 노드 찾기
	{
		parent = del;
		if (fcmp(del +1, key) >0)
		{
			del = del->left;
		}
		else
		{
			del = del->right;
		}
	}
	if (del ==NULL) //찾는 노드가 없다면
	{
		return NULL;  //NULL 반환, 종료
	}
	if (del->right ==NULL)
		//Case #1 오른쪽 자식 노드 없을 때
	{
		son = del->left; //기존 노드의 왼쪽 자식 이어 받음 (NULL이라도 상관 X)
	}
	else if (del->right->left ==NULL)
		//Case #2 오른쪽 자식 노드의 왼쪽 자식 노드가 없을 때
	{
		son = del->right; //기존 노드의 오른쪽 자식 이어 받음
		son->left = del->left; //기존 노드의 왼쪽 자식이 이어 받은 자식의 왼쪽 노드가 된다.
	}
	else
		//Case #3 그 이외 (이전 함수의 Case #3과 유사)
	{
		nexth = del->right;
		while (nexth->left->left !=NULL)
			//삭제할 노드의 오른쪽 자식노드의 하위 노드에서 삭제할 노드와 가장 가까운 값 (오른쪽 에서 가장 왼쪽 노드)찾는 과정
		{
			nexth = nexth->left;//왼쪽으로 계속 이동
		}
		son = nexth->left; //찾은 노드에서 왼쪽으로 한칸 더 아래 노드를 자식(차기 노드)으로 설정
		nexth->left = son->right; //기존 son 자리에서 son 잘라내는 과정
		son->left = del->left; //기존 노드(삭제 예정)의 왼쪽 자식 연결 이어받음
		son->right = del->right; //기존 노드(삭제 예정)의 오른쪽 자식 연결 이어받음
	}
	if (fcmp(key, parent +1) <0 || parent == base)
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
node* _balance(int N, void* a, int width, int* index)
//in-order traverse 기반 tree 생성
//in-order traverse로 이진 트리를 생성하면 균형잡힌 트리를 생성 가능하다.
{
	int nl, nr;
	node* p;
	if (N >0) //종료 조건 #1 하위 노드가 존재할 경우에만 재귀호출
	{
		nl = (N -1) /2; //왼쪽 하위 노드 개수 계산
		nr = N - nl -1; //오른쪽 하위 노드 개수 계산 //root(중심 부모 노드)는 빼고 계산
		p = (node*)malloc(sizeof(node) + width); //새 노드 동적할당
		p->left = _balance(nl, a, width, index); //새 노드의 왼쪽 자식 노드 연결
		memcpy(p +1, (char*)a + width * (*index), width);//새 노드에 정렬된 배열 값 저장
		(*index)++;
		p->right = _balance(nr, a, width, index); //새 노드의 오른쪽 자식 노드 연결
		return p;
	}
	else //종료 조건 #2 leaf 노드의 자식 노드 연결 위해
	{
		return NULL; //자식 노드로 NULL return
	}
}
void _sort(node* p, void* a, int width, int* index)
//in-order traverse 기반 트리 정렬 함수
//in-order traverse 정렬은 노드를 왼쪽->오른쪽 순서대로 level에 관계없이 정렬 가능하다.
{
	if (p !=NULL) // 종료 조건
	{
		_sort(p->left, a, width, index); //왼쪽 자식 노드 탐색
		memcpy((char*)a + width * (*index), p +1, width);//정렬배열에 현재 노드 값 저장
		(*index)++;
		_sort(p->right, a, width, index); //오른쪽 자식 노드 탐색
	}
}
void _deleteall(node* t)
{//post-order traverse 기반 이진트리 동적할당 해제
	if (t !=NULL)
	{
		_deleteall(t->left);
		_deleteall(t->right);
		free(t);
	}
}
void* btv_deleteall(node* base, int* num)
{
	node* t;
	t = base->left;
	_deleteall(t);
	base->left =NULL;
	base->right =NULL;
	*num =0;
	return base;
}
void btv_balance(node* base, int* num, int width)
{
	int index =0;
	void* tmp;
	int ntmp =*num;
	tmp =malloc(width * ntmp);
	index =0;
	_sort(base->left, tmp, width, &index);
	ntmp =*num;
	btv_deleteall(base, num);
	index =0;
	base->left = _balance(ntmp, tmp, width, &index);
	*num = ntmp;
	free(tmp);
}
void btv_sort_list(node* p, void(*fptr)(void*))
{
	int i;
	static int x =0;
	if (p !=NULL)
	{
		x +=2; // position for displaying a node
		// a kind of inorder traverse
		btv_sort_list(p->right, fptr);
		for (i =2; i < x; i++)
		{
			printf(" ");
		}
		fptr(p +1); // print it in the monitor
		btv_sort_list(p->left, fptr);
		x -=2;
	}
}
void _draw_node(node* p, void(*fptr)(void*, char*), int x1, int x2, int y, int mode)
{
	char content[20];
	if (p !=NULL)
	{
		_draw_node(p->left, fptr, x1, (x1 + x2) /2, y + DY, mode);
		fptr(p +1, content);
		fillellipse((x1 + x2) /2, y, textwidth(content) /2 +5, textheight(content) /2 +5);
		outtextxy((x1 + x2) /2, y, content);
		_draw_node(p->right, fptr, (x1 + x2) /2, x2, y + DY, mode);
	}
	else if (mode & TAIL)
		fillellipse((x1 + x2) /2, y, 3, 3);
}
void _draw_link(node* p, int x1, int x2, int y, int mode)
{
	if (p !=NULL)
	{
		lineto((x1 + x2) /2, y);
		_draw_link(p->left, x1, (x1 + x2) /2, y + DY, mode);
		moveto((x1 + x2) /2, y);
		_draw_link(p->right, (x1 + x2) /2, x2, y + DY, mode);
		lineto((x1 + x2) /2, y);
	}
	else if (mode & TAIL)
	{
		lineto((x1 + x2) /2, y);
		moveto((x1 + x2) /2, y);
	}
}
void btv_draw(node* p, void(*fptr)(void*, char*), int mode)
{
	int gd = DETECT, gm;
	initgraph(&gd, &gm, "");
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	setfillstyle(0, BLACK);
	moveto(getmaxx() /2, 10);
	if (mode & LINK)
		_draw_link(p, 5, getmaxx() -5, 10, mode);
	if (mode & NODE)
		_draw_node(p, fptr, 5, getmaxx() -5, 10, mode);
}
