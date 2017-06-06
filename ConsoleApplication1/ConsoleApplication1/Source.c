#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

typedef struct NodeType {
	bool color; //red=true, blk=false
	int key;
	int bh;
	struct NodeType* left;
	struct NodeType* right;
	struct NodeType* p; //parent
}Node;

typedef struct NodeType* NodePtr;

typedef struct TreeType {
	struct NodeType* nil;
	struct NodeType* root;
	int rootbh;
}Tree;

typedef struct TreeType* TreePtr;

// t->nil 생성
NodePtr rbt_nil() {
	NodePtr node = (NodePtr)malloc(sizeof(Node));
	node->color = false;
	node->key = 0;
	node->bh = 0;
	node->left = node;
	node->right = node;
	node->p = node;

	return node;
}

// t 생성
TreePtr rbt_init() {
	TreePtr tree = (TreePtr)malloc(sizeof(Tree));
	tree->nil = rbt_nil();
	tree->root = tree->nil;
	tree->rootbh = 0;
	return tree;
}

// node 생성
NodePtr node_alloc(TreePtr t, int n) {
	NodePtr node = (NodePtr)malloc(sizeof(Node));
	node->color = true;
	node->key = n;
	node->bh = 0;
	node->left = t->nil;
	node->right = t->nil;
	node->p = t->nil;

	return node;
}

// grandparent of node z
NodePtr gp(TreePtr t, NodePtr z) {
	if ((z != t->nil) && (z->p != t->nil))
		return z->p->p;
	else
		return t->nil;
}

// uncle of node z
NodePtr uc(TreePtr t, NodePtr z) {
	NodePtr g = gp(t, z);
	if (g == t->nil)
		return t->nil;
	if (z->p == g->left)
		return g->right;
	else
		return g->left;
}


// black height
int rbt_bh(TreePtr t, NodePtr n) {
	int lh = 0;
	int rh = 0;


	if (n == t->nil)
		return 0;

	if (!(n->color))
		n->bh++;

	if ((n->left == t->nil) && (n->right == t->nil))
		return n->bh;

	if ((n->left) != t->nil)
		lh = rbt_bh(t, n->left);

	if ((n->right) != t->nil)
		rh = rbt_bh(t, n->right);

	if (lh == rh)  //오류 검사
		n->bh += lh;

	return n->bh;
}

// bh reset
void rbt_upbh(TreePtr t, NodePtr root) {
	if (root == t->nil)
		return;

	rbt_upbh(t, root->left);
	root->bh = 0;
	rbt_upbh(t, root->right);
}


void left_rotate(TreePtr t, NodePtr x) {
	if (x->right == t->nil)
		return;

	//set y
	NodePtr y = x->right;

	x->right = x->right->left;

	//turn y's left subtree into x's right subtree
	if (y->left != t->nil)
		y->left->p = x;

	y->p = x->p;

	//link x's parent to y
	if (x->p == t->nil)
		t->root = y;
	else {
		if (x == x->p->left)
			x->p->left = y;
		else
			x->p->right = y;
	}

	//put x on y's left
	y->left = x;
	x->p = y;

}

void right_rotate(TreePtr t, NodePtr y) {
	if (y->left == t->nil)
		return;

	//set x
	NodePtr x = y->left;


	//turn x'right subtree into y's left subtree
	if (x->right != t->nil)
		x->right->p = y;

	y->left = x->right;
	x->p = y->p;

	//link y's parent to x
	if (y->p == t->nil)
		t->root = x;
	else {
		if (y == y->p->left)
			y->p->left = x;
		else
			y->p->right = x;
	}

	//put y on x's right
	x->right = y;
	y->p = x;

}



void rbt_insert_fix_up(TreePtr t, NodePtr z) {
	NodePtr g = gp(t, z);
	NodePtr uncle_pt = uc(t, z);

	while (z->p->color == true && z != t->root) {


		/*  Case : A
		Parent of z is left child of Grand-parent of z */
		if (z->p == z->p->p->left) {

			uncle_pt = z->p->p->right;

			/* Case : 1
			The uncle of z is also red
			Only Recoloring required */
			if (uncle_pt->color == true) {
				z->p->color = false;
				uncle_pt->color = false;
				z->p->p->color = true;
				z = z->p->p; //나중에 검사
			}
			else if (uncle_pt->color == false) {

				/* Case : 2
				z is right child of its parent
				Left-rotation required */
				if (z == z->p->right) {
					z = z->p;
					left_rotate(t, z);
				}

				/* Case : 3
				z is left child of its parent
				Right-rotation required */
				z->p->color = false;
				z->p->p->color = true;
				right_rotate(t, z->p->p);
			}
		}


		/*  Case : B
		Parent of z is right child of Grand-parent of z */
		else
		{

			uncle_pt = z->p->p->left;

			/* Case : 1
			The uncle of z is also red
			Only Recoloring required */
			if (uncle_pt->color == true) {
				z->p->color = false;
				uncle_pt->color = false;
				z->p->p->color = true;
				z = z->p->p; //나중에 검사
			}


			else if (uncle_pt->color == false) {
				/* Case : 2
				z is left child of its parent
				right-rotation required */
				if (z == z->p->left) {
					right_rotate(t, z->p);
					z = z->right;
				}

				/* Case : 3
				z is right child of its parent
				left-rotation required */
				z->p->color = false;
				z->p->p->color = true;
				left_rotate(t, z->p->p);
			}
		}

	}

	t->root->color = false;
}



void rbt_insert(TreePtr t, NodePtr z) {
	NodePtr y = t->nil;
	NodePtr x = t->root;

	// node z의 p 연결
	while (x != t->nil) {
		y = x;
		if (z->key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	z->p = y;


	// p의 node z 연결
	if (y == t->nil)
		t->root = z;
	else if (z->key < y->key)
		y->left = z;
	else
		y->right = z;


	// node z 기본 설정
	z->left = t->nil;
	z->right = t->nil;
	z->color = true;

	rbt_insert_fix_up(t, z);

}



//삭제 연산
NodePtr rbt_find(TreePtr t, NodePtr node, int num) {
	while (node->key != num && node != t->nil) {

		if (node->key < num)
			node = node->right;
		else if (node->key > num)
			node = node->left;

	}
	if (node->key == num)
		return node;

	return t->nil;
}

NodePtr rbt_min(TreePtr t, NodePtr x) {
	if (x == t->nil)
		return x;

	while (x->left != t->nil)
		x = x->left;

	return x;
}

void rbt_transplant(TreePtr t, NodePtr u, NodePtr v) {
	if (u->p == t->nil)
		t->root = v;
	else if (u == u->p->left)
		u->p->left = v;
	else
		u->p->right = v;
	v->p = u->p;
}

void rbt_delete_fixup(TreePtr t, NodePtr x) {
	NodePtr w = NULL;
	while (x != t->root && x->color == false) {
		if (x == x->p->left) {
			w = x->p->right; //sibling

							 //case 1 : x's sibling w is red
			if (w->color == true) {
				w->color = false;
				x->p->color = true;
				left_rotate(t, x->p);
				w = x->p->right;
			}

			else { // x's sibling w is blk

				   //case 2 : both w's children are blk
				if (w->right->color == false) {
					if (w->left->color == false) {
						w->color = true;
						x = x->p;
					}

					//case 3 : w's left is red, w's right is blk
					else if (w->left->color == true) {
						w->left->color = false;
						w->color = true;
						right_rotate(t, w);
						w = x->p->right;
					}
				}

				//case 4 : w's right is red
				if (w->right->color == true) {
					w->color = x->p->color;
					x->p->color = false;
					w->right->color = false;
					left_rotate(t, x->p);
					x = t->root;
				}
			}
		}

		else {
			w = x->p->left;

			//case 1 : x's sibling w is red
			if (w->color == true) {
				w->color = false;
				x->p->color = true;
				right_rotate(t, x->p);
				w = x->p->left;
			}

			else { // x's sibling w is blk

				   //case 2 : both w's children are blk
				if (w->left->color == false) {
					if (w->right->color == false) {
						w->color = true;
						x = x->p;
					}

					//case 3 : w's right is red, w's left is blk
					else if (w->right->color == true) {
						w->right->color = false;
						w->color = true;
						left_rotate(t, w);
						w = x->p->left;
					}
				}

				//case 4 : w's left is red
				if (w->left->color == true) {
					w->color = x->p->color;
					x->p->color = false;
					w->left->color = false;
					right_rotate(t, x->p);
					x = t->root;
				}
			}
		}
	}
	x->color = false;
}

NodePtr rbt_max(TreePtr t, NodePtr x) {
	if (x == t->nil)
		return x;

	while (x->right != t->nil)
		x = x->right;

	return x;
}



void rbt_delete(TreePtr t, NodePtr z) {
	if (z == t->nil)
		return;

	NodePtr y = z;
	NodePtr x = NULL;
	NodePtr zz = NULL;
	int data;
	NodePtr yy = NULL;
	bool y_origin_color = y->color;



	//지우려는 노드 오른쪽 자식만 있으면 그 자식 올리기 + 아예 자식 없는 것 포함
	if (z->left == t->nil) {
		x = z->right;
		rbt_transplant(t, z, z->right);
	}
	//지우려는 노드 왼쪽 자식만 있으면 그 자식 올리기
	else if (z->right == t->nil) {
		x = z->left;
		rbt_transplant(t, z, z->left);
	}
	else {
		//양쪽 자식이 있을 때
		if (z == t->root) {
			zz = rbt_max(t, t->root->left);
			data = zz->key;
			yy = t->root;

			rbt_delete(t, zz);
			if (t->root == yy)
				t->root = zz;
			else {
				rbt_delete(t, yy);
				rbt_insert(t, node_alloc(t, data));
			}
			return;
		}

		//successor 찾고 origin color 저장
		y = rbt_min(t, z->right);
		y_origin_color = y->color;
		x = y->right;
		if (y->p == z)
			x->p = y;
		else {
			rbt_transplant(t, y, y->right);
			y->right = z->right;
			y->right->p = y;
		}
		rbt_transplant(t, z, y);
		y->left = z->left;
		y->left->p = y;
		y->color = z->color;
	}

	if (y_origin_color == false)
		rbt_delete_fixup(t, x);


}


//결과 출력
void rbt_print(TreePtr t, NodePtr x, int lvl) {
	int i = 0;

	if (x == t->nil)
		return;
	if (x->right != t->nil)
		rbt_print(t, x->right, lvl + 1);

	for (i = 0; i < lvl; i++)
		printf("    ");

	if (x->color == true)
		printf("%d[R]\n", x->key);
	else
		printf("%d(B)\n", x->key);

	if (x->left != NULL)
		rbt_print(t, x->left, lvl + 1);

}


void rbt_inorder(TreePtr t, NodePtr root) {
	if (root == t->nil)
		return;
	rbt_inorder(t, root->left);
	printf("%d\n", root->key);
	rbt_inorder(t, root->right);
}

int total(TreePtr t, NodePtr n) {
	int result = 1;

	if (n == t->nil)
		return 0;
	else {
		result += total(t, n->left);
		result += total(t, n->right);
		return result;
	}
}

// tree의 총 black node 수
int rbt_blacknode(TreePtr t, NodePtr n) {
	int result = 1;
	if (n == t->nil)
		return 0;

	if (n->color == false) {
		result += rbt_blacknode(t, n->left);
		result += rbt_blacknode(t, n->right);

	}
	else {
		result--;
		result += rbt_blacknode(t, n->left);
		result += rbt_blacknode(t, n->right);
	}
	return result;
}


// 과제 결과 값
void p(TreePtr t) {
	printf("total = %d\n", total(t, t->root));
	printf("nb = %d\n", rbt_blacknode(t, t->root));
	rbt_upbh(t, t->root);
	printf("bh = %d\n", rbt_bh(t, t->root));
	//rbt_print(t, t->root, 1);
	rbt_inorder(t, t->root);
}

int main() {
	TreePtr t = rbt_init();
	int data;
	int i = 0;

	FILE * fp = fopen("input.txt", "r");
	while (fscanf_s(fp, "%d", &data) != EOF) {
		i++;
		if (data > 0)
			rbt_insert(t, node_alloc(t, data));
		else if (data < 0) {
			data = -1 * data;
			rbt_delete(t, rbt_find(t, t->root, data));
		}
		else if (data = 0)
			break;

	
	}
	p(t);
	fclose(fp);



	return;
}