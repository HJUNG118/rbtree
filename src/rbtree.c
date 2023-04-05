#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 트리 생성
  // TODO: initialize struct if needed
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  return p;
} 

void del_node(rbtree *t, node_t *n){
  if(n != t->nil){
    del_node(t, n->left); // 제일 왼쪽 리프 노드부터 메모리 해제
    del_node(t, n->right);
    free(n);
    n = NULL; 
//n의 주소를 NULL로 초기화하는 이유: 
//메모리는 해제되더라도 혹시 다른 프로그램이 돌렸을 때 우연히 해당 메모리를 사용하게 되면 문제가 발생할 수 있기 때문이다.
  }
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  del_node(t, t->root);
  free(t->nil); //nil노드가 가리키는 노드 해제
  free(t);
  t = NULL;
}

void left_rotation(rbtree *t, node_t *x){ // x는 부모가 바뀌어야 하는 노드
  node_t *y = x->right;
  x->right = y->left;
  if(y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil){
    t->root = y;
  }
  else if(x == x->parent->left){
    x->parent->left = y;
  }
  else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotation(rbtree *t, node_t *x){
  node_t *y = x->left;
  x->left = y->right;
  if(y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil){
    t->root = y;
  }
  else if(x == x->parent->right){
    x->parent->right = y;
  }
  else{
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
}

void rb_insert_fixup(rbtree *t, node_t *z){
  while(z->parent->color == RBTREE_RED){
    if(z->parent == z->parent->parent->left){ //부모가 조부모의 왼쪽 자식이라면
      node_t *uncle = z->parent->parent->right; //삼촌은 조부모의 오른쪽 자식
			// case1. 삼촌이 red라면
      if(uncle->color == RBTREE_RED){
				// 부모, 삼촌은 black, 조부모는 red로 색변환
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; 
				//할아버지를 기준으로 다시 rb트리의 속성을 만족하는지 확인하기 위해 z 갱신      
      }
      else{ // case2. 삼촌이 red가 아니고, 새 노드가 부모의 오른쪽 자식이라면
        if(z == z->parent->right){
          z = z->parent;
          left_rotation(t, z); // 새 노드가 새 노드의 부모와 연결되도록 회전
        } 
				// case3. 새 노드가 부모의 왼쪽 자식이라면
					// 부모는 black, 조부모는 red로 색변환
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotation(t, z->parent->parent); // 조부모가 새 노드 부모의 자식이 되도록 회전
      }
    }
    else{
      node_t *uncle = z->parent->parent->left;
      if(uncle->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;     
      }
      else{ 
        if(z == z->parent->left){
          z = z->parent;
          right_rotation(t, z);
        } 
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }      
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *y = t->nil; //삽입될 위치가 갱신될 포인터
  node_t *x = t->root; //root부터 삽입위치를 확인하기 위해 t->root로 x초기화
  while(x != t->nil){ //while문을 돌면서 갱신되는 y는 새 노드가 삽입되어야 할 노드의 부모 노드 갱신
    y = x; //삽입할 노드의 부모를 계속 갱신한다.
    if(key < y->key){
      x = x->left;
    }
    else {
      x = x->right;
    }
  } // x가 nil노드일 때 즉 마지막 자신까지 갔을 때가 삽입할 때이다
  node_t *z = (node_t *)malloc(sizeof(node_t)); //새로운 노드 생성
  z->key = key;
  z->parent = y; // 갱신된 y가 새 노드의 부모 노드
  if(y == t->nil) //트리가 비어있는 상태라면
    t->root = z;// 새 노드는 루트가 된다.
  else if(z->key < y->key)
    y->left = z;
  else
    y->right = z;
	// 새 노드의 자식은 닐노드
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED; // 새 노드는 RED
  rb_insert_fixup(t, z); // 새 노드를 삽입하고, 변경된 속성을 수정
  return t->root; // 루트 노드 반환
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *tmp = t->root; // ptr에 루트 노드의 주소를 담는다.(t->root를 직접 참조하면 t->root의 값이 변해버린다.)
  while(tmp != t->nil && tmp->key != key){
    if(tmp->key > key){
      tmp = tmp->left;
    }
    else{
      tmp = tmp->right;
    }
  }
  if(tmp->key == key){
    return tmp; // 해당 키가 존재하는 노드를 반환한다.
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *n = t->root;
  while(n->left != t->nil){
    n = n->left;
  }
  return n;
}

node_t *node_min(const rbtree *t, node_t *n){ //삭제할 노드의 successor를 찾는다.
  node_t *ptr;
  ptr = n;
  while(ptr->left != t->nil){
    ptr = ptr->left;
  }
  return ptr;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *n = t->root;
  while(n->right != t->nil){
    n = n->right;
  }
  return n;
}

void rb_transplant(rbtree *t, node_t *u, node_t *v){ // 해당 노드와 자식이 넘어옴
	// 삭제될 노드의 부모와 삭제될 노드의 자식을 연결
  if(u->parent == t->nil){
    t->root = v;
  }
  else if(u == u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void rb_erase_fixup(rbtree *t, node_t *target)
{
  node_t *u;
  while(target != t->root && target->color == RBTREE_BLACK){
    if(target->parent->left == target){ // 타겟이 부모노드의 왼쪽 자식일 떄
      u = target->parent->right; // 삼촌은 부모노드의 오른쪽이 된다
			// case1. 삼촌이 빨간색일 때
      if(u->color == RBTREE_RED){
        u->color = RBTREE_BLACK;
        target->parent->color = RBTREE_RED;
        left_rotation(t,  target->parent);
        u = target->parent->right;
      }
      // case2. 삼촌의 자식이 모두 검은색일 때
      else if(u->right->color == RBTREE_BLACK && u->left->color == RBTREE_BLACK){ 
        u->color = RBTREE_RED;
        target = target->parent;
      }
      else{ // case3. 삼촌은 검은색이고, 삼촌의 자식이 한개라도 빨간색일 때
        if(u->right->color == RBTREE_BLACK){ // 오른쪽 자식이 검은색일 때
          u->color = RBTREE_RED;
          u->left->color = RBTREE_BLACK;
          right_rotation(t, u);
          u = target->parent->right;
        }
        // case4. 삼촌은 검은색이고 오른쪽 자식이 빨간색일 때
        u->color = target->parent->color;
        target->parent->color = RBTREE_BLACK;
        u->right->color = RBTREE_BLACK;
        left_rotation(t, target->parent); // 조부모를 기준으로 회전시킨다.
        target = t->root;
      }
    }
    else{ // 타겟(doubly black)이 부모노드의 오른쪽 자식일 때
      u = target->parent->left;
      if(u->color == RBTREE_RED){
        u->color = RBTREE_BLACK;
        target->parent->color = RBTREE_RED;
        right_rotation(t, target->parent);
        u = target->parent->left;
      }
      if(u->left->color == RBTREE_BLACK && u->right->color == RBTREE_BLACK){
        u->color = RBTREE_RED;
        target = u->parent;
      }
      else{ 
        if(u->left->color == RBTREE_BLACK){
          u->color = RBTREE_RED;
          u->right->color = RBTREE_BLACK;
          left_rotation(t, u);
          u = target->parent->left;
        }
        u->color = target->parent->color;
        target->parent->color = RBTREE_BLACK;
        u->left->color = RBTREE_BLACK;
        right_rotation(t, target->parent);
        target = t->root;
      }
    }
  }
  target->color = RBTREE_BLACK;
}
  
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;
  node_t *ptr;
  color_t y_original_color = p->color;
  // 1. 왼쪽이 닐노드(오른쪽도 nil node여도 됨)
  if(p->left == t->nil){
    ptr = p->right;
    rb_transplant(t, p, p->right); // 삭제될 노드의 부모와 삭제될 노드의 자식을 연결
  }
  // 2. 오른쪽이 닐노드(왼쪽도 nil node여도 됨)
  else if(p->right == t->nil){
    ptr = p->left;
    rb_transplant(t, p, p->left);
  }
  // 3. 둘다 노드
  else{
    y = node_min(t, p->right); // p의 successor
    y_original_color = y->color; // 삭제될 노드의 자식이 둘이면 삭제될 컬러는 successor 컬러다
    ptr = y->right;
    // 3-1. y가 p의 자식일 때
    if(y->parent == p){
      ptr->parent = y;
    }
    // 3-2. 그 외에는 y의 부모노드를 y->right과 연결시켜야 한다.
    else{
      rb_transplant(t, y, y->right); // 삭제된 노드로 대체될 y의 자식을 y부모의 자식으로 이식
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y); // y를 삭제될 노드의 자식으로 이식
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if(y_original_color == RBTREE_BLACK){
    rb_erase_fixup(t, ptr); // ptr은 doubly black
  }
  free(p); //p노드 삭제
  return 0;
}

int inorder_rbtree(node_t *root, key_t *res, const rbtree *t, int i){
  if(root == t->nil){
      return i;
   }
  i = inorder_rbtree(root->left, res, t, i);
  res[i] = root->key;
  i += 1;
  i = inorder_rbtree(root->right, res, t, i);
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  node_t *tmp = t->root;
  inorder_rbtree(tmp, arr, t, 0);
  return 0;
}