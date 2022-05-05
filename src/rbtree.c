#include "rbtree.h"

#include <stdlib.h>

/** RB Tree 구조체 생성 **/
rbtree *new_rbtree(void) {
  // calloc(블록 개수, 자료형 크기): 메모리 내용 0으로 초기화해서 시작 포인터 반환
  // 트리 메모리 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // sentinel node 메모리 할당
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  // sentinel node는 검정색
  nil_node->color = RBTREE_BLACK; 
  p->nil = nil_node;
  p->root = p->nil; 

  return p;
}

/* 노드와 노드 이하의 메모리 반환 */
void free_node(rbtree *t, node_t *node) {
  // 후위 순회 방식 : 왼쪽 자식 -> 오른쪽 자식 -> 뿌리
  if (node != t->nil){
    free_node(t, node->left);
    free_node(t, node->right);
    free(node);
    node = NULL;
  }

  /* 전위, 중위 순회 방식은 루트 노드가 자식 노드 이전에 삭제되어 매핑되지 않아 Segmentation fault 발생 */
  // 중위 순회 방식: 왼쪽 자식 -> 뿌리 -> 오른쪽 자식
  // if (node != t->nil){
  //   free_node(t, node->left);
  //   free(node);  // 할당됐던 메모리가 가용되는 상태로 해제됨
  //   node = NULL; // *node는 여전히 해당 주소 가리키므로 포인터를 NULL로 설정
  //   free_node(t, node->right);
  // }

  // 전위 순회 방식 : 뿌리 -> 왼쪽 자식 -> 오른쪽 자식
  // if (node != t->nil){
  //   free(node);
  //   node = NULL;
  //   free_node(t, node->left);
  //   free_node(t, node->right);
  // }

}


/* RB Tree 구조체가 차지했던 메모리 반환 */
void delete_rbtree(rbtree *t) {
  node_t *node = t->root;

  // 루트가 t->nil이 아닐 때
  if (node != t->nil){
    // node들 메모리 반환
    free_node(t, node);
  }
  // sentinel 노드 메모리 반환
  free(t->nil);
  t->nil = NULL; // 포인터 변수의 이미 반환된 힙 블록 내 데이터 참조 멈추기
  
  // rbtree 구조체 메모리 반환
  free(t);
  t = NULL;

  /* 내가 짰던 코드 */
  // if (node == t->nil){ // 루트 노드가 nil일 때
  //   free(node);
  //   node = NULL;
  // } else {
  //   free_node(t, node);
  // }

  // free(t->nil); // 이 경우, 내가 지우고 싶은 닐 노드와 루트노트가 t->nil일 경우 모두 free가 되므로 free를 두번 해줘서 오류가 뜸
  // t->nil = NULL; 
  // free(t);
  // t = NULL;

}

/* x 기준 좌회전 */
void left_rotation(rbtree *t, node_t *x){
  // x의 오른쪽 자식노드 y 선언
  node_t *y = x->right;

  x->right = y->left; // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮김

  if (y->left != t->nil){
    y->left->parent = x; // x를 y의 왼쪽 서브트리의 부모가 가리키는 곳으로
  }

  y->parent = x->parent; // 원래 x의 부모를 y의 부모로 연결

  if (x->parent == t->nil){
    t->root = y; // x의 부모가 비어있었다면 y가 노드의 루트가 됨
  } 
  else if (x == x->parent->left){ // x의 부모입장에서 x가 왼쪽 자식이었다면
    x->parent->left = y; // y를 x의 부모의 왼쪽 자식으로
  }
  else{
    x->parent->right = y; // y를 x의 부모의 오른쪽 자식으로
  }
  y->left = x; // x를 y의 왼쪽 자식으로 놓음
  x->parent = y; // y를 x의 부모로 둠
}

/* x 기준 우회전 */
void right_rotation(rbtree *t, node_t *x){
  // x의 왼쪽 자식노드 y 선언
  node_t *y = x->left;

  x->left = y->right; // y의 오른쪽 서브트리를 x의 왼쪽 서브트리가 가리키는 곳으로 옮김

  if (y->right != t->nil){
    y->right->parent = x; // x가 가리키는 곳을 y의 오른쪽 서브트리의 부모
  }
  y->parent = x->parent; // 원래 x의 parent가 가리키는 곳을 y의 부모가 가리키는 곳으로 옮김 (원래 x의 부모를 y로 연결)

  if (x->parent == t->nil){ // x의 부모가 비어있었다면 y가 노드의 루트가 됨
    t->root = y;
  }
  else if (x == x->parent->left){ // x의 부모입장에서 x가 왼쪽 자식이었다면
    x->parent->left = y; // y를 x의 부모의 좌측 자식이 가리키는 곳으로
  }
  else{
    x->parent->right = y;
  }
  y->right = x; // x를 y의 오른쪽으로 놓음
  x->parent = y; // y를 x의 부모로 둠
  
}

/* 특성 중 위반되는 게 있는지 확인, 특성 복구 */
void insertion_fixup(rbtree *t, node_t *z){
  node_t *y = t->nil; // y를 트리의 nil 노드로 선언 ?

  // z.p의 색깔이 B이 되면 while문 자체를 종료시킴 (더이상 적색 노드 두 개가 연달아서 나타나지 X)
  while (z->parent->color == RBTREE_RED)
  {
    // z.p가 왼쪽 자식일 때
    if (z->parent == z->parent->parent->left)
    {
      y = z->parent->parent->right; // y는 오른쪽 삼촌
      // 경우 1. 삼촌이 RED인 경우
      if (y->color == RBTREE_RED)
      { 
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } 
      else // 삼촌이 BLACK인 경우
      {
        // 경우 2. z가 오른쪽 자손일 때
        if (z == z->parent->right)
        { 
          z = z->parent;
          left_rotation(t, z); // z 기준에서 할아버지까지 가는데 꺾인 걸 펴주기 위해 좌회전
        }
        // 경우 3. z가 왼쪽 자식일 때
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotation(t, z->parent->parent); // 레드를 넘기기 위해 색깔 바꾸고 우회전
      }
    }
    else // z.p가 오른쪽 자식일 때
    {
      y = z->parent->parent->left; // y는 왼쪽 삼촌
      // 삼촌이 RED인 경우
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else // 삼촌이 BLACK인 경우
      {
        // z가 왼쪽 자손일 때
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotation(t, z); 
        }

        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent); 
      }
    } 
  } // z.p의 색깔이 B가 됐을 때 while문 종료

  // 트리의 루트노드를 검은 노드로 변환
  // 이유?
  // while문에서 삽입된 노드 z가 루트일 때가 고려 안되어 있음
  // 경우1이 끝났을 때, z.p.p가 루트노드일 경우, 더이상 p가 없으므로 while문 깨고 나가는데 그 때 조부모가 RED이므로, BLACK으로 변환
  t->root->color = RBTREE_BLACK; 
}

/* RB Tree 구조체에 key 추가 */
node_t *rbtree_insert(rbtree *t, const key_t key) { // 삽입되는 값은 key
  node_t *y = t->nil; // nil 노드 y 선언
  node_t *x = t->root; // 루트 노드 x 선언
  node_t *z = (node_t *)calloc(1, sizeof(node_t)); // size(node_t)만큼의 데이터 크기를 1개만큼 할당 
  z->key = key; // z 포인터의 key 멤버변수가 전달받은 상수 key라는 값을 가짐

  while (x != t->nil) { // 루트노드가 nil이 아니면 반복문 실행
    y = x; // y 위치에 루트노드 주소 저장
    if (z->key < x->key){ // 삽입값 < 루트노드 값
      x = x->left;
    }
    else{
      x = x->right;
    }
  }
  
  z->parent = y; // y를 z의 parent가 가리키는 곳으로 대입 (== z의 부모노드를 y로 변경)

  if (y==t->nil){ // 루트노드가 nil이라면
    t->root = z;
  }
  else if (z->key < y->key){
    y->left = z;
  }
  else{
    y->right = z;
  }
  z->left = t->nil; // rbtree 구조 유지 위해 z 자식 노드들 nil로 설정하고 색깔을 RED로
  z->right = t->nil;
  z->color = RBTREE_RED;

  // insert하고 특성 위반되는 게 있는지 확인한 다음 복구해줌
  insertion_fixup(t, z);

  return t->root; // 리턴해서 null 값인지 assert에서 체크해주려고
}

/* RB Tree 내 해당 key가 있는지 탐색하고 있으면 node pointer 반환, 없으면 NIL 반환 */
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // 트리의 현재 루트노드 포인터 변수 current 선언
  node_t *current = t->root;

  // RBT의 속성 사용해서 대소비교로 탐색
  while (current != t->nil)
  {
    if (current->key == key){
      return current;
    }
    if (current->key < key){
      current = current->right;
    } else if (current->key > key){
      current = current->left;
    }
  }
  return NULL;
}

/* RB Tree 중 최소값을 가진 node pointer 반환 */
node_t *rbtree_min(const rbtree *t) {
  // 트리의 현재 루트노드 포인터 변수 current 선언
  node_t *current = t->root;

  while (current != t->nil && current->left != t->nil){ // current만 쓰면 current->left가 nil일 때 NULL이 반환됨
    current = current->left;
  }

  return current;
}

/* RB Tree 중 최대값을 가진 node pointer 반환 */
node_t *rbtree_max(const rbtree *t) {
  // 트리의 현재 루트노드 포인터 변수 current 선언
  node_t *current = t->root;
  // 루트 노드가 null일 경우
  if (current == t->nil)
  {
    return NULL;
  }

  while (current->right != t->nil) // current만 쓰면 current->right가 nil일 때 NULL이 반환됨
  {
    current = current->right;
  }
  
  return current;

}

/* 삭제할 노드 u와 u를 대체할 노드 v의 연결 관계를 바꿔줌*/
/* u를 삭제하고 u의 부모와 v를 연결 */
void rb_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) // u의 부모가 nil이면 v가 루트가 됨
  {
    t->root = v;
  }
  else if (u == u->parent->left) // u가 u의 부모의 왼쪽 자식이면
  {
    u->parent->left = v; // u의 부모의 왼쪽 자식이 가리키는 곳을 v가 가리키는 곳으로 수정
  }
  else 
  {
    u->parent->right = v; // u의 부모의 오른쪽 자식이 가리키는 곳을 v가 가리키는 곳으로 수정
  }
  v->parent = u->parent; 
  // v의 부모가 가리키는 곳을 u의 부모가 가리키는 곳으로 수정
  // v의 부모로 u의 부모를 가짐
}

/* 삭제된 색깔이 BLACK인 경우 rbtree 속성이 위반될 수 있으므로 복구해줌 */
/* x(extra black)를 t->root로 올리거나 red가 있는 곳으로 보내서 red and black으로 만들어준 다음, 순수한 흑색으로 만들어주면 끝 */
void delete_fixup(rbtree *t, node_t *x) { // z가 삭제되고 대체된 값 x
  while(x != t->root && x->color == RBTREE_BLACK){ // while문 내부에서 x는 이중 흑색 노드를 가리킴
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right; // w는 x의 형제
      // 경우 1. w가 적색일 때
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK; // x.p를 기준으로 좌회전하기 위해 루트가 black이 될 수 있도록 색깔 바꿈
        x->parent->color = RBTREE_RED;
        left_rotation(t,x->parent); 
        w = x->parent->right; // 색깔이 흑색인 새로운 형제노드
      }
      // 경우 2. w의 자식들이 모두 흑색일 때
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent; // extra black을 x.p로 옮겨줌 -> x.p가 적색이었으면 red and black되어 상황 종료 / 흑색이었으면 계속 while문
      }
      else {
        // 경우 3. w의 왼쪽 자식이 적색이고 오른쪽 자식이 흑색일 때
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotation(t, w);
          w = x->parent->right;
        }
        // 경우 4. w의 오른쪽 자식이 적색일 때
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK; // x.p의 경우, x에 있는 extra black을 해소시켜줘야해서 red인데 red and black되어 순수한 흑색됨
        w->right->color = RBTREE_BLACK; 
        left_rotation(t, x->parent);
        x = t->root; // 경우 4까지 돌면 자동으로 x = t->root가 됨
      }
    }
    else 
    {
      node_t *w = x->parent->left;

      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotation(t,x->parent);
        w = x->parent->left;
      }

      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if (w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotation(t, w);
          w = x->parent->left;
        }

        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotation(t, x->parent);
        x = t->root;

      } 
    }
  }
  // x가 바로 루트일 때 while문 돌지 않으므로 black으로 처리해줘야
  x->color = RBTREE_BLACK;
}

/* p로 지정된 node 삭제 및 메모리 반환 */
int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p; // 해주는 이유? z가 하나 이하의 자식이 있을 때 처리해주기 위해 y를 p의 위치로 옮김 -> p가 삭제됨
  node_t *x;
  color_t y_original_color = y->color;

  if (p->left == t->nil)
  {
    x = p->right;
    rb_transplant(t, p, p->right); // p를 삭제하고 p의 부모와 p->right의 관계가 조정되는 함수
  }
  else if (p->right == t->nil)
  {
    x = p->left;
    rb_transplant(t, p, p->left);
  }
  else
  {
    // 자식 둘 다 nil노드 아닐 때 오른쪽 서브트리에서 가장 작은 값이 successor
    // y = rbtree_min(p->right);  매개변수 자료형 맞지 않음
    y = p->right;
    while (y->left != t->nil){ 
      y = y->left;
    }
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p)
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right);
      y->right = p->right; // y의 right가 가리키는 곳을 z의 right가 가리키는 곳으로 수정해줌
      y->right->parent = y; // 아까 전에 옮긴 y의 right가 가리키는 곳의 parent를 y가 가리키는 곳으로 수정해줌 (역방향 연결)
    }
    rb_transplant(t, p, y); // p를 삭제하고 그 자리에 y를 넣어줌
    y->left = p->left; // y의 left가 가리키는 곳에 p의 left가 가리키는 곳을 넣어줌
    y->left->parent = y; // p의 자식을 y에 연결시켜줌 (역방향)
    y->color = p->color; // y가 p의 색깔을 물려받음
  }
  
  if (y_original_color == RBTREE_BLACK){ // 삭제되는 색이 검정색일 때 문제 발생
    delete_fixup(t, x); // extra black을 가지는 대체된 노드 x
  }

  // 메모리 반환 함수 호출
  free(p);

  return 0; 
}

void inorder_array(const rbtree *t, node_t *p, key_t *arr, const size_t n, int *cnt) {
  if (p == t->nil || *cnt > n){ // 재귀를 멈춰주는 *cnt > n
    return;
  }
  
  // 중위 순회 방식
  if (p->left != t->nil){ 
    inorder_array(t, p->left, arr, n, cnt);
  }
  if (*cnt < n){ // arr에 n개까지 들어감
    arr[(*cnt)++] = p->key; // cnt 주소가 가리키는 값을 가져오고 1을 더해줌
  }
  if (p->right != t->nil){
    inorder_array(t, p->right, arr, n, cnt);
  }

}

// RB Tree 내용을 key 순서대로 주어진 arr 반환. arr 크기는 n으로 주어지고 n보다 크기가 큰 경우엔 n개까지만 반환
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *current = t->root;

  // 루트 노드가 null일 경우
  if (current == t->nil){
    return 0;
  }

  int cnt = 0;
  // 중위 순회
  inorder_array(t, t->root, arr, n, &cnt); 

  return 0;
}
