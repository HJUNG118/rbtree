#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  return p;
}

void del_node(rbtree *t, node_t *n){
  if(n != t->nil){
    del_node(t, n->left);
    del_node(t, n->right);
    free(n);
    n = NULL; //n의 주소를 NULL로 초기화하는 이유: 메모리는 해제되더라도 혹시 다른 프로그램이 돌렸을 때 우연히 해당 메모리를 사용하게 되면 문제가 발생할 수 있다.
  }
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  del_node(t, t->root);
  free(t->nil); //nil노드가 가리키는 노드 해제
  free(t);
  t = NULL;
}

void left_rotation(rbtree *t, node_t *x){ // x는 삽입된 코드
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
    if(z->parent == z->parent->parent->left){//부모가 조부모의 왼쪽이라면
      node_t *uncle = z->parent->parent->right;
      if(uncle->color == RBTREE_RED){ // 삼촌이 red라면
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; //할아버지를 기준으로 다시 rb트리의 속성을 만족하는지 확인하고 정렬해줘야 하기 때문이다.        
      }
      else{ // 삼촌이 red가 아니라면
        if(z == z->parent->right){ // 자식이 부모의 오른쪽 자식이라면
          z = z->parent;
          left_rotation(t, z);
        } // 자식이 부모의 왼쪽 자식이라면
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          right_rotation(t, z->parent->parent);
      }
    }

    else{ //부모가 조부모의 오른쪽이라면
      node_t *uncle = z->parent->parent->left;
      if(uncle->color == RBTREE_RED){ // 삼촌이 red라면
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; //할아버지를 기준으로 다시 rb트리의 속성을 만족하는지 확인하고 정렬해줘야 하기 때문이다.        
      }
      else{ // 삼촌이 red가 아니라면
        if(z == z->parent->left){ // 자식이 부모의 왼쪽 자식이라면
          z = z->parent;
          right_rotation(t, z); //돌리고 색 바꾼 뒤 left_rotation 해줘야 한다.
        } 
        // 자식이 부모의 오른쪽 자식이라면
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
  node_t *y = t->nil;
  node_t *x = t->root; //삽입할 노드의 부모 노드의 주소
  while(x != t->nil){
    y = x; //삽입할 노드의 부모를 계속 갱신한다.
    if(key < y->key){
      x = x->left;
    }
    else {
      x = x->right;
    }
  } // x가 nil노드일 때 즉 마지막 자신까지 갔을 때가 삽입할 때이다
  node_t *z = (node_t *)malloc(sizeof(node_t));
  z->key = key;
  z->parent = y;
  if(y == t->nil) //트리가 비어있는 상태라면
    t->root = z;
  else if(z->key < y->key){//부모노드보다 작으면 왼쪽 자식이 된다.
    y->left = z;
  }  
  else{ //부모노드보다 크면 오른쪽 자식
    y->right = z;
  }
  z->left = t->nil; //sentinel 노드를 가리킨다
  z->right = t->nil; //sentinel 노드를 가리킨다
  z->color = RBTREE_RED; //RBTREE_RED는 0이다.
  rb_insert_fixup(t, z);
  return t->root; // z반환해야 되나?
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *tmp = t->root;
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

node_t *node_min(const rbtree *t, node_t *n){
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

void rb_erase_fixup(rbtree *t, node_t *target){
  while(target != t->root && target->color == RBTREE_BLACK){
    node_t *u;
    if(target->parent->left == target){ // 타겟이 부모노드의 왼쪽 자식일 떄
      u = target->parent->right; // 삼촌은 부모노드의 오른쪽이 된다
      if(u->color == RBTREE_RED){ // case1. 삼촌이 오른쪽 빨간색일 때
        u->color = RBTREE_BLACK;
        target->parent->color = RBTREE_RED; // 삼촌의 부모는 연결되어있지 않다.
        left_rotation(t, target->parent);
        u = target->parent->right;
      }
      if(u->right->color == RBTREE_BLACK && u->left->color == RBTREE_BLACK){ // case2. 삼촌의 자식이 모두 검은색일 때
        u->color = RBTREE_RED;
        target = target->parent;
      }
      else{
        if(u->right->color == RBTREE_BLACK){
           // case3. 삼촌은 검은색이고, 삼촌의 자식이 한개라도 빨간색일 때
          u->color = RBTREE_RED;
          u->left->color = RBTREE_BLACK;
          right_rotation(t, u);
          u = target->parent->right;
        }// case4. 삼촌은 검은색이고 오른쪽 자식이 빨간색일 때
        u->color = target->parent->color;
        target->parent->color = RBTREE_BLACK;
        u->right->color = RBTREE_BLACK;
        left_rotation(t, target->parent); // 조부모를 기준으로 회전시킨다.
        target = t->root;
      }
    }
    else{ // 타겟이 부모노드의 오른쪽 자식일 때
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
  // 1. 왼쪽이 닐노드
  if(p->left == t->nil){
    ptr = p->right;
    rb_transplant(t, p, p->right); // p의 부모와 p->right와 연결해야한다
  }
  // 2. 오른쪽이 닐노드
  else if(p->right == t->nil){
    ptr = p->left;
    rb_transplant(t, p, p->left);
  }
  // 3. 둘다 노드
  else{
    y = node_min(t, p->right); // p의 successor
    y_original_color = y->color; // 삭제될 노드의 자식이 둘이면 삭제될 컬러는 successor 컬러다
    ptr = y->right;
    // 1. y가 p의 자식일 때
    if(y->parent == p){
      ptr->parent = y;
    }
    // 2. 그 외에는 y의 부모노드를 y->right과 연결시켜야 한다
    else{
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y); // p의 부모노드를 y와 연결시킨다.
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if(y_original_color == RBTREE_BLACK){
    rb_erase_fixup(t, ptr); // ptr은 doubly black
  }
  free(p); //p노드 삭제
  return 0;
  // 삭제하려는 노드의 자녀가 없거나 하나라면 삭제되는 색은 삭제되는 노드의 색
  // 삭제하려는 노드의 자녀가 둘이라면 삭제되는 색=삭제되는 노드의 successor
    // 삭제된 색이 black이라면 tree를 재정렬 해야한다
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