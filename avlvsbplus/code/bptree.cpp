#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<cstring>
using namespace std;

const int L = 16;

const int min = 2;
const int Max = 4;

struct node{
	int num; //to keep track of how much the node is filled
	int nodetype; //whether the node is an internal node or not.

	char rec[Max][16];
	struct node *ptr[Max + 1];
};

struct forans{
	struct node *pfd;
	char *mrecord;

};

int glflag = 0 ;

class Bptree{

	struct node *Root;
	struct forans *glb;

	public:
		
		struct forans * insert_record(char *a,struct node *p,char *b, struct node *q,int *fl, int mainflag);
		struct forans * insert_into_root(char *a,struct node *p,char *b, struct node *q,int *fl);
		struct forans * insert_into_node(struct forans *tpq,char *abc,struct node *p,char *b, struct node *q,int *fl);
		struct node * split_child(struct node *ts,char *b);
		struct node * split_node(struct node *ts,char *b, struct node *xil);

		int record_search(struct node *p,char *b);
		void display_btree(struct node *p);

		void print_node(struct node *p);
};


int Bptree:: record_search(struct node *p , char *b){
	if(p == NULL){
		return -1;
	}
	int i = 0 ,ans = -1; 
	for( i = 0 ; i < p->num ;i++){
		if(strcmp(p->rec[i],b)==0){
			return 1;
		}
		else if(strcmp(p->rec[i],b) > 0){
			ans = i;
			break;	
		}
	}
	return record_search(p->ptr[i],b);

}

void Bptree::print_node(struct node *p){
	int i;

	if(p == NULL){
		return;
	}
	printf("Printing the records\n");
	for(i = 0 ; i < p->num;i++){
		printf("%s\n",p->rec[i]);
	}
}
void Bptree::display_btree(struct node *p){
	int i,j;
	int levels = 1;
	while(p->ptr[0]!=NULL){
		p = p->ptr[0];
		levels++;

	}

	int ans = 0;
	struct node *temp = p ;

	while(1){

		if(temp==NULL){
			break;
		}
		for(i = 0;i<temp->num;i++){
			printf("%s\n",temp->rec[i]);
			ans++;
		}

		printf("\n");

		temp = temp->ptr[temp->num];
	}
	printf("Total ans = %d\n",ans);
	
	printf("Total levels = %d\n",levels);
}

struct forans * Bptree::insert_into_node(struct forans *tpq,char *abc,struct node *p,char *b, struct node *q,int *fl){

	int i, j , k;
	int ans = -1;

	char *a = tpq->mrecord;

	struct node *rkh = tpq->pfd;

	if(p->num < Max){
		if(p->num == 0){
			exit(0);
		}

		else if(strcmp(p->rec[p->num-1],a) < 0){
			strcpy(p->rec[p->num] , a);
			p->ptr[p->num+1] = rkh;
			p->num++;
		}

		else{
			for(i = 0;i < p->num; i++){
				if(strcmp(p->rec[i],a) > 0){
					break;
				}
			}
			ans = i;

			for(j = p->num ; j > ans;j--){
				strcpy(p->rec[j],p->rec[j-1]);
			}
			for(j = p->num ; j > ans;j--){
				p->ptr[j+1] = p->ptr[j];
			}
			strcpy(p->rec[ans],a);
			p->ptr[ans+1] = rkh;
			p->num++;
		}
		return NULL;


	}

	struct node *n1 = split_node(p,a,rkh);

	struct forans *t2 = (struct forans *)malloc(sizeof(struct forans));
	t2->pfd = n1;
	t2->mrecord = (char *)malloc(L*sizeof(char));
	strcpy(t2->mrecord,n1->rec[0]);

	for(i = 0 ; i < n1->num-1;i++){
		strcpy(n1->rec[i],n1->rec[i+1]);
	}
	for(i = 0 ; i < n1->num ; i++){
		n1->ptr[i] = n1->ptr[i+1];

	}
	n1->ptr[n1->num] = NULL;
	n1->num--;

	return t2;

}
struct forans * Bptree::insert_into_root(char *a, struct node *p , char *b, struct node *q, int *fl){

	int i, j , k;
	int ans = -1;
	// if node is not full
	if(p->num < Max){

		if(p->num == 0){
			exit(0);
		}

		else if(strcmp(p->rec[p->num-1],a) < 0){
			strcpy(p->rec[p->num] , a);
			p->ptr[p->num+1] = p->ptr[p->num];
			p->ptr[p->num] = NULL;
			p->num++;
		}

		//if the element is not the last element
		else{
			for(i = 0;i < p->num; i++){
				if(strcmp(p->rec[i],a) > 0){
					break;
				}
			}
			ans = i;

			for(j = p->num ; j > ans;j--){
				strcpy(p->rec[j],p->rec[j-1]);
			}
			for(j = p->num ; j > ans;j--){
				p->ptr[j+1] = p->ptr[j];
			}
			strcpy(p->rec[ans],a);
			p->ptr[ans] = NULL;
			p->num++;
		}

		return NULL;
	}

	//else we have to split
	*fl = 1; //meaning that a leaf node has been split

	struct node *n1 = split_child(p,a);

	struct forans *t2 = (struct forans *)malloc(sizeof(struct forans));
	t2->pfd = n1;
	t2->mrecord = (char *)malloc(L * sizeof(char ));
	strcpy(t2->mrecord,n1->rec[0]);

	return t2;
}

struct forans * Bptree::insert_record(char *a, struct node *p, char *b, struct node *q, int *fl,int mainflag){

	int i,j ,k;
	int fg = 0;
	if(p == NULL && mainflag == 0 &&glflag == 0){
		struct node *mt;
		mt = (struct node *) malloc(sizeof(struct node));

		mt->nodetype = 0;
		mt->num = 1;
		strcpy(mt->rec[0],a);
		mt->ptr[0] = NULL;
		mt->ptr[1] = NULL;
		Root = mt;

		mainflag++;
		glflag++;

		struct forans *tempar = (struct forans *)malloc(sizeof(struct forans));
		tempar->pfd = Root;
	
		glb = tempar;
		return glb;
	
	}

	//filling the root node
	else if(mainflag == 1&&glflag == 1){

		int rn;
		int i,j,k;
		struct forans *nw1 = insert_into_root(a,Root,b,q,&rn);
	
		if(nw1 == NULL){
			return glb;
		}

		else if(nw1!=NULL){
			struct node *tp2;
			tp2 = (struct node *)malloc(sizeof(struct node));
			tp2->num = 1;

			struct node *new1 = nw1->pfd;

			strcpy(tp2->rec[0],new1->rec[0]);
			
			tp2->nodetype = 0;

			tp2->ptr[0] = p;
			tp2->ptr[1] = new1;

			struct node *yy;
			yy = tp2->ptr[0];
			yy->nodetype = 2;

			yy = tp2->ptr[1] ;
			yy->nodetype = 2;

			yy = tp2->ptr[tp2->num];

			Root = tp2;
			glflag++;
			glb->pfd = Root;
			return glb;
		}
	}

	//if the node is root node
	else if(glflag>1 && p->nodetype!=2){


		struct node *kpt = p;

		int ans = 0;
		int tpfg = 0;
		for(i = 0 ;i < kpt->num;i++){
			if(strcmp(kpt->rec[i],a) > 0){
				tpfg = 1;
				ans = i;
				break;
			}
		}

		struct node *naya = NULL;

		if(tpfg == 1){
			naya = kpt->ptr[ans];
		}
		else{
			naya = kpt->ptr[kpt->num];
		}


		int bhejde = 0;
		char *toc;

		struct forans *ctr;
		ctr = insert_record(a,naya,toc, NULL,&bhejde, 2);
		
	
		if(ctr == NULL && p == Root){
			return glb;
		}
		else if(ctr == NULL && p!=Root){
			return NULL;
		}
		else if(ctr!= NULL && p!=Root){
			char *jj;
			int rn3 = 0;
			return insert_into_node(ctr,a,p,b,q,&rn3);
		}
		else if(ctr != NULL && p== Root){
			// split the root and then
			int rn2 = 0;
			struct forans *nw1 = insert_into_node(ctr,a,Root,b,q,&rn2);

			if(nw1 == NULL){
				return glb;
			}

			else if(nw1!=NULL){
				struct node *tp2;
				tp2 = (struct node *)malloc(sizeof(struct node));
				tp2->num = 1;
				struct node *new1 = nw1->pfd;
				strcpy(tp2->rec[0],nw1->mrecord);
				tp2->nodetype = 0;

				tp2->ptr[0] = Root;
				tp2->ptr[1] = new1;

				struct node *yy;
				yy = tp2->ptr[0];
				yy->nodetype = 1;

				yy = tp2->ptr[1] ;
				yy->nodetype = 1;

				yy = tp2->ptr[tp2->num];

				Root = tp2;
				glflag++;

				glb->pfd = Root;

				return glb;
			}


		}

	}

	else if(p->nodetype == 2){
		int asd = 0;
		return insert_into_root(a,p,b,q,&asd);
	}
}

struct node * Bptree::split_node(struct node *ts, char *b , struct node *xil){
	struct node *temp = ts->ptr[ts->num];

	int i,j;
	struct node *w1;
	w1 = (struct node *)malloc(sizeof(struct node));
	w1->num = 0 ;
	w1->nodetype = 1;

	struct node *k1;
	k1 = (struct node *)malloc(sizeof(struct node));
	k1->num = ts->num ;


	for(i = 0 ;i<ts->num;i++){
		strcpy(k1->rec[i],ts->rec[i]);
	}
	for(i = 0 ;i<ts->num+1;i++){
		k1->ptr[i] = ts->ptr[i];
	}
	int ans = 0;
	i = 0;
	int pot = 0;
	int flag = 0;

	for(j = 0;j<Max/2;j++){
		if(strcmp(b,k1->rec[pot])<0 && flag == 0){
			flag  = 1;
			strcpy(ts->rec[i],b);
			ts->ptr[i+1] = xil;
			i++;
			continue;	
		}
		strcpy(ts->rec[i],k1->rec[pot]);
		ts->ptr[i+1] = k1->ptr[pot+1];
		i++;
		pot++;
	}

	ts->num = Max/2;

	i=0;
	for(j = 0 ; j< Max - (Max/2) ;j++){
		if(strcmp(b,k1->rec[pot])<0 && flag == 0){
			flag  = 1;
			strcpy(w1->rec[i],b);
			w1->ptr[i+1] = xil;
			i++;
			continue;	
		}
		strcpy(w1->rec[i],k1->rec[pot]);
		w1->ptr[i+1] = k1->ptr[pot+1];
		i++;
		pot++;
	}
	
	if(flag == 0){
		strcpy(w1->rec[i],b);
		w1->ptr[i+1] = xil;
	}
	else{
		strcpy(w1->rec[i],k1->rec[pot]);
		w1->ptr[i+1] = k1->ptr[pot+1];

	}
	i++;


	w1->num = Max - (Max/2) + 1;

	return w1;

}
struct node * Bptree::split_child(struct node *ts,char *b){

	int flag = 0;
	if(ts == NULL){
		exit(1);
	}
	struct node *temp = ts->ptr[ts->num];

	struct node *w1 = NULL;

	w1 = (struct node *)malloc(sizeof(struct node));
	w1->num = 0;
	w1->nodetype = 2;
	
	struct node *k1;
	k1 = (struct node *)malloc(sizeof(struct node));
	k1->num = ts->num ;

	int i,j;
	for(i = 0 ;i<ts->num;i++){
		strcpy(k1->rec[i],ts->rec[i]);
	}
	for(i = 0 ;i<ts->num+1;i++){
		k1->ptr[i] = NULL;
		ts->ptr[i] = NULL;
	}

	int ans = 0;
	i = 0;
	int pot = 0;
	for(j = 0;j<Max/2;j++){
		if(strcmp(b,k1->rec[pot])<0 && flag == 0){
			flag  = 1;
			strcpy(ts->rec[i],b);
			i++;
			continue;	
		}
		strcpy(ts->rec[i],k1->rec[pot]);
		i++;
		pot++;
	}

	ts->num = Max/2;

	i=0;
	for(j = 0 ; j< Max - (Max/2) ;j++){
		if(strcmp(b,k1->rec[pot])<0 && flag == 0){
			flag  = 1;
			strcpy(w1->rec[i],b);
			i++;
			continue;	
		}
		strcpy(w1->rec[i],k1->rec[pot]);
		i++;
		pot++;
	}
	if(flag == 0){
		strcpy(w1->rec[i],b);
	}
	else{
		strcpy(w1->rec[i],k1->rec[pot]);
	}
	i++;
	w1->num = Max - (Max/2) + 1;
	ts->ptr[ts->num] = w1;
	for(i = 0 ;i<w1->num;i++){
		w1->ptr[i] = NULL;
	}
	w1->ptr[w1->num] = temp;
	return w1;
}

FILE *fp;
FILE *tw ;
char file[100];
void open_btree(){
	strcpy(file , "in1000000");
	fp = fopen(file , "r");
	tw = fopen("ans3.txt","w");
}


int main(){

	Bptree B;

	struct forans *pl;

	int tr;

	char *abc;
	char *kk;
	kk = (char *)malloc(16*sizeof(char));

	int df = 0;
	
	int n = 0;

	open_btree();

	fread(kk,1 , L ,fp);
	kk[L-1] = '\0';
	pl = B.insert_record(kk, NULL,  abc,NULL, &tr,0);

	fprintf(tw, "%s",kk);
	fprintf(tw,"\n");


	//form of the getNext function
	while((n = fread(kk, 1, L,fp))> 0) {
		kk[L-1] = '\0';
		df = 1;
		df = B.record_search(pl->pfd,kk);
		if(df == 1){
			continue;
		}

		pl = B.insert_record(kk, pl->pfd,  abc,NULL, &tr,1);

		//form of the write function
		fprintf(tw, "%s",kk);
		fprintf(tw,"\n");

	}
//	B.display_btree(pl->pfd);

	fclose(fp);
	fclose(tw);
	return 0;
}



