#include <stdio.h> 
#include <stdlib.h> 

int max(int a,int b)
{
	if(a>b)
		return a;
	else
		return b;
}
typedef struct AvlNode 
{ 
	int data,height; 
	struct AvlNode *left_child, *right_child; 
}AvlNode; 

AvlNode *root; 

int search(AvlNode *root,int key)
{
	if (root==NULL)
		return 0;
	else if(key==root->data)
		return 1;
	else if(key<root->data)
		return search(root->left_child,key);
	else
		return search(root->right_child,key);
}
int height(AvlNode *t)
{
	if(t==NULL)
		return 0;
	else
		return t->height;
}
void preOrder(AvlNode *root)
{
	if(root==NULL)
		return;
	else
	{
		printf("%d ", root->data);
		preOrder(root->left_child);
		preOrder(root->right_child);
	}
}

AvlNode *minValueNode(AvlNode* t)
{
	AvlNode *current=t;
	while(current->left_child!=NULL)
		current=current->left_child;
	return current;
}

AvlNode *rightRotate(AvlNode *y)
{
	AvlNode *child=y->left_child;
	AvlNode *temp=child->right_child;
	child->right_child=y;
	y->left_child=temp;
	y->height=max(height(y->left_child),height(y->right_child))+1;
	child->height=max(height(child->left_child),height(child->right_child))+1;
	return child;
}
AvlNode *leftRotate(AvlNode *x)
{
	AvlNode *child=x->right_child;
	AvlNode *temp=child->left_child;
	child->left_child=x;
	x->right_child=temp;
	x->height=max(height(x->left_child),height(x->right_child))+1;
	child->height=max(height(child->left_child),height(child->right_child))+1;
	return child;
}
int getBalance(AvlNode *t)
{
	if (t==NULL)
		return 0;
	return height(t->left_child)-height(t->right_child);
}
AvlNode* rotate_LL(AvlNode *parent) 
{ 
	AvlNode *child = parent->left_child; 
	parent->left_child = child->right_child; 
	child->right_child = parent; 
	return child; 
} 


AvlNode* rotate_RR(AvlNode *parent) 
{ 
	AvlNode *child = parent->right_child; 
	parent->right_child = child->left_child; 
	child->left_child = parent; 
	return child; 
} 


AvlNode* rotate_RL(AvlNode *parent) 
{ 
	AvlNode *child = parent->right_child; 
	parent->right_child = rotate_LL(child); 
	return rotate_RR(parent); 
} 


AvlNode* rotate_LR(AvlNode *parent) 
{ 
	AvlNode *child = parent->left_child; 
	parent->left_child = rotate_RR(child); 
	return rotate_LL(parent); 
} 

int get_height(AvlNode *node) 
{ 
	int height=0; 
	if(node != NULL) 
		height = 1+max(get_height(node->left_child),get_height(node->right_child)); 
	return height; 
} 


int get_balance(AvlNode *node) 
{ 
	if(node == NULL) return 0; 
	return get_height(node->left_child) - get_height(node->right_child); 
} 


AvlNode* balance_tree(AvlNode **node) 
{ 
	int height_diff= get_balance(*node); 

	if(height_diff > 1) 
	{ 
		if(get_balance((*node)->left_child) > 0) 
			*node = rotate_LL(*node); 
		else  
			*node = rotate_LR(*node); 
	} 
	else if(height_diff < -1)
	{ 
		if(get_balance((*node)->right_child) < 0) 
			*node = rotate_RR(*node); 
		else 
			*node = rotate_RL(*node); 
	} 
	return *node; 
} 

AvlNode* avl_add(AvlNode **root,int key) 
{ 
	if(*root == NULL) 
	{ 
		*root = (AvlNode*)malloc(sizeof(AvlNode)); 
		(*root)->data = key;     
		(*root)->left_child = (*root)->right_child = NULL; 
	} 
	else if(key < (*root)->data) 
	{ 
		(*root)->left_child = avl_add(&((*root)->left_child),key); 
		(*root) = balance_tree(root);  
	} 
	else if(key > (*root)->data) 
	{ 
		(*root)->right_child = avl_add(&((*root)->right_child), key); 
		(*root) = balance_tree(root); 
	} 
	return *root; 
} 

AvlNode *deleteNode(AvlNode *root,int key)
{
	if (root==NULL)
		return root;
	if (key<root->data)
		root->left_child=deleteNode(root->left_child,key);
	else if(key>root->data)
		root->right_child=deleteNode(root->right_child,key);
	else
	{
		if((root->left_child==NULL)||(root->right_child==NULL))
		{
			AvlNode *temp=root->left_child?root->left_child:root->right_child;
			if(temp==NULL)
			{
				temp=root;
				root=NULL;
			}
			else 
				*root=*temp; 
			free(temp);
		}
		else
		{
			AvlNode* temp=minValueNode(root->right_child);
			root->data=temp->data;
			root->right_child=deleteNode(root->right_child,temp->data);
		}
	}
	if(root==NULL)
		return root;
	root->height=max(height(root->left_child),height(root->right_child))+1;
	int balance=getBalance(root);
	if (balance>1&&getBalance(root->left_child)>=0)
		return rightRotate(root);
	if (balance>1&&getBalance(root->left_child)<0)
	{
		root->left_child=leftRotate(root->left_child);
		return rightRotate(root);
	}
	if (balance<-1&&getBalance(root->right_child)<=0)
		return leftRotate(root);
	if (balance<-1&&getBalance(root->right_child)>0)
	{
		root->right_child=rightRotate(root->right_child);
		return leftRotate(root);
	}
	return root;
}
int main() 
{
	int option,n,i,value;
	AvlNode *root=NULL;
	while(1)
	{
		printf("1.insert\n2.delete\n3.search\n4.print\n5.quit\n");
		scanf("%d",&option);
		if(option==1)
		{
			printf("no. of insertions: ");
			scanf("%d",&n);
			for(i=0;i<n;i++)
			{
				printf("key to be inserted: ");
				scanf("%d",&value);
				avl_add(&root,value);
			}
		}
		if(option==2)
		{
			printf("key to be deleted: ");
			scanf("%d",&value);
			root=deleteNode(root,value);
		}
		if(option==3)
		{
			printf("search : ");
			scanf("%d",&value);
			if(search(root,value)!=0)
				printf("found\n");
			else
				printf("not found\n");
		}
		if(option==4)
		{
			printf("preorder traversal: ");
			preOrder(root);
			printf("\n");
		}
		if(option==5)
			break;
	}
	return 0;
}
