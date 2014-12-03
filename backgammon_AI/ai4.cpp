//Rupinder :D
//Aayush
#include<iostream>
#include<stack>
#include<cmath>
#include<map>
#include<set>
#include<string.h>
#include<stdio.h>
#include<vector>
#include<math.h>
#include<string>
#include<algorithm>
#include<iterator>
#include<iomanip>
#include<limits.h>
#include<numeric>
#include<float.h>
using namespace std;

#define Pi 3.14159265358979323846264338327950288419716939937510582
typedef long long int lld;
typedef vector<int> vi;
typedef pair<int,int> pii;
#define sz size()
#define MOD 1000000007
#define pb push_back
#define mp make_pair
#define pf push_front
#define ppb pop_back
#define ff first
#define ss second
#define rep(i,n) for(i=0;i<n;i++)
#define all(a) a.begin(),a.end()
lld modpow(lld a,lld n,lld temp){lld res=1,y=a;while(n>0){if(n&1)res=(res*y)%temp;y=(y*y)%temp;n/=2;}return res%temp;}
lld gcd(lld a,lld b){if(a==0)return(b);else return(gcd(b%a,a));}
pair<float,pii > secondmove(vi &d,int roll);
pair<pii,pii > firstmove(vi &d,int roll[])
{
	bool bear=1;
	for(int i=0;i<=18;i++)
		if(d[i]>0)
			bear=0;
	float curh=0,maxh=-FLT_MAX;
	float toth;
	pair<float,pii> temp;
	pair<pii,pii> high=mp(mp(0,0),mp(0,0));
	if(bear){
		for(int j=0;j<2;j++)
		{
			maxh=-FLT_MAX;
			bool empty=d[25-roll[j]]<=0,behind=0;
			for(int i=19;i<25-roll[j];i++)
				if(d[i]>0)behind=1;
			for(int i=19;i<=24;i++)
			{
				curh=0;
				if(!empty ||(empty&&behind)){
					if(i>25-roll[j]||d[i]<=0)continue;
					if(i+roll[j]>=25)
						curh=curh+i+roll[j];
					else if(d[i+roll[j]]<-1)
						continue;
					else if(d[i+roll[j]]==1)
						curh+=i+roll[j];
					else if(d[i+roll[j]]==-1)
						curh+=24-(i+roll[j]);
					else if(d[i+roll[j]]==0)
						curh-=(i+roll[j]);
					if(d[i+roll[j]]>1)
						curh+=i+roll[j];
					if(d[i]>1)
						curh-=i;
					if(d[i]==2)
						curh-=2*i;
					if(curh>maxh){
						maxh=curh;
						if(j==0)
						{
							high.ff.ff=i;
							high.ff.ss=i+roll[j];
						}else{
							high.ss.ff=i;
							high.ss.ss=i+roll[j];
						}
					}
				}else if(empty&&(!behind))
				{
					if(d[i]>0){
						maxh=curh;
						if(j==0)
						{
							high.ff.ff=i;
							high.ff.ss=i+roll[j];
						}else{
							high.ss.ff=i;
							high.ss.ss=i+roll[j];
						}
						break;
					}
				}
			}
			if(j==0){
				d[high.ff.ff]-=1;
				d[high.ff.ss]+=1;
			}else{
				d[high.ff.ff]+=1;
				d[high.ff.ss]-=1;
			}
		}
	}else if(d[0]>0)
	{
		d[0]--;
		for(int j=0;j<2;j++)
		{
			curh=0;
			if(d[roll[j]]>-1)
			{
				d[roll[j]]++;
				if(d[roll[j]]==2)
					curh+=roll[j];//pair creation
				else if(d[roll[j]]>2)
					curh+=roll[j]/3;//pile creation
				else if(d[roll[j]]==1)
					curh-=roll[j]/4;//single move
				temp=secondmove(d,roll[j^1]);
				d[roll[j]]--;
				if(temp.ff==-FLT_MAX)
					toth=curh;
				else
					toth=curh+temp.ff;
				if(toth>maxh)
				{
					maxh=toth;
					if(temp.ff==-FLT_MAX)
						high=mp(mp(0,roll[j]),mp(0,0));
					else
						high=mp(mp(0,roll[j]),temp.ss);
				}
			}
			else if(d[roll[j]]==-1)
			{
				curh+=24-roll[j];//cut
				d[roll[j]]+=2;
				temp=secondmove(d,roll[j^1]);
				d[roll[j]]-=2;
				if(temp.ff==-FLT_MAX)
					toth=curh;
				else
					toth=curh+temp.ff;
				if(toth>maxh)
				{
					maxh=toth;
					if(temp.ff==-FLT_MAX)
						high=mp(mp(0,roll[j]),mp(0,0));	
					else
						high=mp(mp(0,roll[j]),temp.ss);
				}
			}
		}
		d[0]++;
		//return best moves
	}else{

		for(int i=1;i<=24;i++)
		{
			curh=0;
			if(i>6&&i<=18)
				curh+=i/5;
			if(d[i]>0)
			{
				if(d[i]==2)
					curh-=i;//pair destroy
				else if(d[i]>2)
					curh-=i/3;//pile destroy
				else if(d[i]==1)
					curh+=i/4;//single move
				d[i]--;
			}else
				continue;
			float temph=curh;
			for(int j=0;j<2;j++)
			{
				curh=temph;
				if(i+roll[j]>24)continue;
				if(d[i+roll[j]]>-1){
					d[i+roll[j]]++;
					if(d[i+roll[j]]==2)
						curh+=i+roll[j];//pair creation
					else if(d[i+roll[j]]>2)
						curh+=(i+roll[j])/3;//pile creation
					else if(d[i+roll[j]]==1)
						curh-=i/4;//single move
					temp=secondmove(d,roll[j^1]);
					d[i+roll[j]]--;
					if(temp.ff==-FLT_MAX)
						toth=curh;
					else
						toth=curh+temp.ff;
					if(toth>maxh)
					{
						if(temp.ff==-FLT_MAX)
						{
							if(maxh==-FLT_MAX)
							{
								high=mp(mp(i,i+roll[j]),mp(0,0));
								maxh=toth;
							}
						}else
						{
							maxh=toth;
							high=mp(mp(i,i+roll[j]),temp.ss);
						}
					}
				
				}else if(d[i+roll[j]]==-1){
					curh+=24-(i+roll[j]);//cut
					d[i+roll[j]]+=2;
					temp=secondmove(d,roll[j^1]);
					d[i+roll[j]]-=2;
					if(temp.ff==-FLT_MAX)
						toth=curh;
					else
						toth=curh+temp.ff;
					if(toth>maxh)
					{
						if(temp.ff==-FLT_MAX)
						{
							if(maxh==-FLT_MAX)
							{
								high=mp(mp(i,i+roll[j]),mp(0,0));
								maxh=toth;
							}
						}else
						{
							maxh=toth;
							high=mp(mp(i,i+roll[j]),temp.ss);
						}
					}
				}
			}
			d[i]++;
		}
	}
	return high;
}
pair<float,pii > secondmove(vi &d,int roll)
{
	bool bear=1;
	for(int i=0;i<=18;i++)
		if(d[i]>0)
			bear=0;
	float maxh=-FLT_MAX;
	float curh=0;
	if(bear){
		pii high;
		bool empty=d[25-roll]<=0,behind=0;
		for(int i=19;i<25-roll;i++)
			if(d[i]>0)behind=1;
		for(int i=19;i<=24;i++)
		{
			curh=0;
			if(!empty ||(empty&&behind)){
				if(i>25-roll||d[i]<=0)continue;
				if(i+roll>=25)
					curh=curh+i+roll;
				else if(d[i+roll]<-1)
					continue;
				else if(d[i+roll]==1)
					curh+=i+roll;
				else if(d[i+roll]==-1)
					curh+=24-(i+roll);
				else if(d[i+roll]==0)
					curh-=(i+roll);
				if(d[i+roll]>1)
					curh+=i+roll;
				if(d[i]>1)
					curh-=i;
				if(d[i]==2)
					curh-=2*i;
				if(curh>maxh){
					maxh=curh;
					high.ff=i;
					high.ss=i+roll;
				}
			}else if(empty&&(!behind))
			{
				if(d[i]>0){
					high.ff=i;
					high.ss=i+roll;
					break;
				}
			}
		}
		return mp(maxh,high);
	}
	if(d[0]>0)
	{
		if(d[roll]<-1)
			return mp(-FLT_MAX,mp(0,0));
		else{
			if(d[roll]==1)
				curh+=roll;//pair creation
			else if(d[roll]>1)//pile creation
				curh+=roll/3;
			if(d[roll]==-1)//cut
				curh+=24-roll;
			else if(d[roll]==0)//single move
				curh-=roll/4;
			return mp(curh,mp(0,roll));
		}
	}else{
		float maxh=-FLT_MAX;
		int p1,p2;
		for(int i=1;i<=24;i++)
		{
			curh=0;
			if(i>6&&i<=18)
				curh+=i/5;
			if(i>18)
				curh-=i/6;
			if(d[i]>0&&d[i+roll]>=-1&&i+roll<=24)
			{
				//calc health
				if(d[i]==2)
					curh-=i;//pair destroy
				else if(d[i]>2)
					curh-=i/3;//pile destroy
				else if(d[i]==1)
					curh+=i/4;//single move
				if(d[i+roll]==-1)//cut
					curh+=24-(i+roll);
				else if(d[i+roll]==1)//pair create
					curh+=i+roll;
				else if(d[i+roll]>1)
					curh+=(i+roll)/3;//pile create
				else if(d[i+roll]==0)//single move
					curh-=(i+roll)/4;
				if(curh>maxh)
				{
					maxh=curh;
					p1=i;
					p2=i+roll;
				}
			}
		}
		return mp(maxh,mp(p1,p2));
	}
}
int main()
{
	std::ios_base::sync_with_stdio( false );
	int i,j,a[33],b[33],roll[3];
	string bar;
	vi board;
	board.pb(0);
	rep(i,24)
	{
		cin>>a[i];
		board.pb(a[i]);
	}
	board.pb(0);
	for(int i=0;i<20;i++)
		board.pb(0);
	cin>>bar>>roll[0]>>roll[1];
	rep(i,bar.sz)
		if(bar[i]=='a')
			board[0]++;
	if(bar[0]=='z')
		board[0]=0;
	//calculate all possible first moves, with the ranks
	if(roll[0]<roll[1])
		swap(roll[0],roll[1]);
	pair<pii,pii> fmove;
	fmove=firstmove(board,roll);
	if(fmove.ss.ff==0&&fmove.ss.ss!=0)
		swap(fmove.ff,fmove.ss);
	//cout<<fmove.ff.ff<<" "<<fmove.ff.ss<<"\n";
	//cout<<fmove.ss.ff<<" "<<fmove.ss.ss<<"\n";
	if(fmove.ff.ff==0&&fmove.ff.ss==0)
		cout<<"pass\n";
	else
	{
		if(fmove.ff.ff==0)
			cout<<"Z ";
		else
			cout<<fmove.ff.ff<<" ";
		if(fmove.ff.ss>=25)
			cout<<"O\n";
		else
			cout<<fmove.ff.ss<<"\n";
	}
	if(fmove.ss.ff==0&&fmove.ss.ss==0)
		cout<<"pass\n";
	else
	{
		if(fmove.ss.ff==0)
			cout<<"Z ";
		else
			cout<<fmove.ss.ff<<" ";
		if(fmove.ss.ss>=25)
			cout<<"O\n";
		else
			cout<<fmove.ss.ss<<"\n";
	}

	return 0;
}
