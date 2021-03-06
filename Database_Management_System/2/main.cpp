#include<iostream>
#include<sstream>
#include<string>
#include<stdio.h>
#include<algorithm>
#include<stdlib.h>
#include<deque>
#include<fstream>
#include<cstring>
#include<typeinfo>
#include<vector>
#include<dirent.h>
using namespace std;

typedef struct metadata
{
    string name;
    int num;
    int size;
    int records[100];
    string attribute[100];
}metadata;

typedef struct page
{
    int id;
    int modified;
    char *data;
}page;

typedef struct query
{
    string type;
    string columns;
    string tables;
    string condition;
    string groupby;
    string having;
    string orderby;
    string distinct;
    string attributes;
}query;

typedef vector<page> vp;
typedef vector<vp> vvp;

class DBSystem
{
    public: 
        metadata tables[100];              // To store metadata for tables
        vvp pages;
        int mapping[2][10000];
        deque<page> LRU;
        int num_pages,pagesize,table_index,page_index;
        string filePath;
        // pagesize = LRU size
        // num_pages = Maximum number of pages
        // table_index = Number of tables
        // page_index = Number of pages
        // pages = Structure to store all data


        /////////////////////////////////////////////////////////////////////
        void readConfig(string configFilePath)
        {
            int i=0,k,idx;
            table_index = 0;
            string line,word[300];
            ifstream file1(configFilePath.c_str());
            while(getline(file1,line,'\n'))
            {
                stringstream ssin(line);
                while(ssin.good())                // File is read and parsed into words
                    ssin>>word[i++];
            }

            for(int k=0;k<i;k++)
            {
                if((word[k].compare("PAGE_SIZE"))==0)
                    pagesize=atoi(word[k+1].c_str());
                if((word[k].compare("NUM_PAGES"))==0)
                    num_pages=atoi(word[k+1].c_str());
                if((word[k].compare("PATH_FOR_DATA"))==0)
                    filePath = word[k+1];
                if((word[k].compare("BEGIN"))==0)
                {
                    k++;
                    tables[table_index].name = word[k++];

                    string tmp = filePath;
                    tmp.append(tables[table_index].name);
                    tables[table_index].name = tmp;

                    idx = 0;
                    while( word[k].compare("END")!=0 )
                    {
                        string tmp;
                        size_t pos = word[k].find(",");

                        ///////// Building metadata for each table ///////////////
                        tables[table_index].attribute[idx] = word[k].substr(0,pos);
                        tmp = word[k].substr(pos+1);

                        if( !tmp.compare("char") )
                            tables[table_index].records[idx] = 1;
                        else if( !tmp.compare("int") )
                            tables[table_index].records[idx] = 2;
                        else if( !tmp.compare("float") )
                            tables[table_index].records[idx] = 3;
                        else 
                            tables[table_index].records[idx] = 4;

                        k++;
                        idx++;
                        //////////////////////////////////////////////////////////
                    }
                    tables[table_index].num = idx;
                    table_index++;
                }

            }
		file1.close();
        }
        ////////////////////////////////////////////////////////////////////////



        ////////////////////////////////////////////////////////////////////////
        void populateDBInfo()
        {
            int i,j,k,idx;
            page_index = 0;
            int remaining = pagesize;

            // Iterate through all tables
            for(i=0;i<table_index;i++)
            {
                int num = 0;
                vector<page> temp;
                temp.resize(1);
                temp[num].data = new char[pagesize]();
                temp[num].id = page_index;

                string line,record[10000];
                remaining = pagesize;

                // Get all records of current table
                int length;
                idx = 0;
                tables[i].name.append(".csv");
                ifstream file(tables[i].name.c_str());
                while(getline(file,line,'\n'))
                    record[idx++] = line;

                mapping[0][page_index] = 0;

                // Iterate through the records
                for(j=0;j<idx;j++)
                {
                    length = record[j].length()+1;
                    // Check whether record can fit in the page
                    if( length < remaining )                        
                    {
                        strcat(temp[num].data,record[j].c_str());
                        strcat(temp[num].data,"+");
                        remaining -= length;
                    }
                    else
                    {
                        mapping[1][page_index] =  j;

                        page_index++;
                        num++;
                        temp.resize(temp.size()+1);
                        temp[num].data = new char[pagesize]();
                        temp[num].id = page_index;

                        mapping[0][page_index] = j;
                        remaining = pagesize;
                        j--;
                    }
                }


                mapping[1][page_index] =  j-1;
                page_index++;

                pages.push_back(temp);
                temp.clear();

            }
            /*  for(i=0;i<pages.size();i++)
                {
                k = pages[i].size();
                for(j=0;j<k;j++)
                cout << pages[i][j].id << endl << strlen(pages[i][j].data) << endl << pages[i][j].data << endl;
                cout << endl;
                }*/
        }
        ////////////////////////////////////////////////////////////////////////



        ////////////////////////////////////////////////////////////////////////
        void getRecord(string tableName, int recordId)
        {
            ////////// Extracting relevant page ID
            int i,j,k;

            string tmp = filePath;
            tmp.append(tableName);
            tableName = tmp;
            tableName.append(".csv");

            for(i=0;i<table_index;i++)
                if( !tables[i].name.compare( tableName ) )
                    break;

            int l = 0;
            int r = pages[i].size() - 1;
            int mid = (l+r)/2;

            while( l < r )
            {
                int s = mapping[0][pages[i][mid].id];
                int t = mapping[1][pages[i][mid].id];

                if( recordId >= s && recordId <= t )
                    break;
                else if( recordId > t)
                    l = mid+1;
                else
                    r = mid-1;
                mid = (l+r)/2;
            }

            ////////// Searching for required page in LRU

            static int empty = num_pages;

            if( empty )
            {
                int flag = 0;
                j = LRU.size();
                for(k=0;k<j;k++)
                {
                    if( LRU[k].id == mid )
                    {
                        cout << "HIT\n";
                        page x = LRU[k];
                        flush(x);
                        LRU.erase(LRU.begin()+k);
                        LRU.push_front(x);
                        return;
                    }
                }

                j = pages[i].size();
                for(k=0;k<j;k++)
                    if( pages[i][k].id == mid )
                    {
                        LRU.push_front( pages[i][k] );
                        cout << "MISS: " << pages[i][k].id << endl;
                    }
                empty--;
                return;
            }
            else
            {
                int flag = 0;
                j = LRU.size();
                for(k=0;k<j;k++)
                    if( LRU[k].id == mid )
                    {
                        flag = 1;
                        break;
                    }

                if( !flag )
                {
                    j = pages[i].size();
                    for(k=0;k<j;k++)
                        if( pages[i][k].id == mid )
                            break;

                    page tmp = LRU[LRU.size() - 1];
                    flush(tmp);
                    int x = LRU[LRU.size()-1].id;
                    LRU.pop_back();
                    LRU.push_front( pages[i][k] );
                    cout << "MISS " << x << endl;
                }
                else
                {
                    page tmp = LRU[k];
                    flush(tmp);
                    LRU.erase(LRU.begin()+k);
                    LRU.push_front( tmp );

                    cout << "HIT\n";
                }
            }

        }
        ////////////////////////////////////////////////////////////////////////



        ////////////////////////////////////////////////////////////////////////
        void insertRecord(string tableName,string record)
        {
            int i,j,k,p,q,n,tmp;
            int len = record.length()+1;

            string temp = filePath;
            temp.append(tableName);
            tableName = temp;
            tableName.append(".csv");

            // Extract the table id from tableName
            for(i=0;i<table_index;i++)
                if( !tables[i].name.compare(tableName) )
                    break;

            j = pages[i].size() - 1;
            k = pages[i][j].id;
            tmp = strlen(pages[i][j].data);

            n = LRU.size();

            // Search for the page id in LRU
            for( p=0; p<n ;p++ )
            {
                if( LRU[p].id == k )
                {
                    if( pagesize - strlen(LRU[p].data) > len )
                    {
                        strcat( LRU[p].data , record.c_str() );
                        strcat( LRU[p].data , "+" );
                        LRU[p].modified = 1;
                        return;
                    }
                    else
                    {
                        //cout << "Asdf\n";
                        page tmp;
                        tmp.data = new char[pagesize]();
                        strcat(tmp.data, record.c_str() );
                        strcat(tmp.data, "+");
                        cout << i << endl;
                        pages[i].push_back(tmp);
                        if( LRU.size() > pagesize )
                            LRU.pop_back();
                        LRU.push_front(tmp);
                        return;
                    }
                }
            }

            // If not found in LRU
            if( pagesize - strlen( pages[i][j].data ) > len )
            {
                strcat( pages[i][j].data , record.c_str() );
                strcat( pages[i][j].data , "+" );

                pages[i][j].modified = 1;

                if( LRU.size() > pagesize )
                    LRU.pop_back();
                LRU.push_front(pages[i][j]);
            }
            else
            {
                page tmp;
                tmp.data = new char[pagesize]();
                tmp.modified = 1;
                strcat(tmp.data, record.c_str() );
                strcat(tmp.data, "+");
                pages[i].push_back(tmp);
                if( LRU.size() > pagesize )
                    LRU.pop_back();

                LRU.push_front(tmp);
            }
        }
        ////////////////////////////////////////////////////////////////////////



        ////////////////////////////////////////////////////////////////////////
        // Function which takes a page and then writes it's contents into the 
        // file if modified bit is set
        void flush( page p )
        {
            if (p.modified != 1) return;

            int i,j,m,n;
            int x = p.id;

            n = pages.size();
            for(i=0;i<n;i++)
            {
                m = pages[i].size();
                for( j=0;j<m;j++ )
                    if( pages[i][j].id == x )
                        break;

                if( pages[i][j].id == x )
                    break;
            }

            FILE *f1;
            string name = tables[i].name;
            f1 = fopen( name.c_str(), "wb" );
            char *line = new char[pagesize]();
            int index = 0;
            for(j=0;j<m;j++)
            {
                int len = strlen( pages[i][j].data );
                for(int k=0;k<len;k++)
                {
                    if( pages[i][j].data[k] == '+' )
                        fputc( '\n' , f1 );
                    else
                        fputc( pages[i][j].data[k] , f1);
                }
            }
            fclose( f1 );

            p.modified = 0;

        }

        void flushPages()
        {
            int i,j,n;
            int u,v , k,l;
            n = LRU.size();

            for(i=0;i<n;i++)
                flush( LRU[i] );
            LRU.clear();
        }
        ////////////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////////////
        void printQuery( query q )
        {
            if(q.type.compare("select")==0)
            {
                cout << "Querytype: " << q.type << endl;
                cout << "Tablename: " << q.tables << endl;
                if(q.columns[q.columns.size()-1]==',')
                    q.columns.erase(q.columns.size()-1,1);
                cout << "Columns: " << q.columns << endl;
                cout << "Distinct: " << q.distinct << endl;
                cout << "Condition: " << q.condition << endl;
                cout << "Orderby: "<< q.orderby << endl;
                cout << "Groupby: " << q.groupby << endl;
                cout << "Having: " << q.having << endl;
            }
            if(q.type.compare("create")==0)
            {
                cout << "Querytype: " << q.type << endl;
                cout << "Tablename: " << q.tables << endl;
                cout << "Attributes: " << q.attributes << endl;
            }

        }
        ////////////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////////////
        query parse( string input )
        {
            vector<string> words;

            cout << input << endl;
            istringstream StrStream(input);
            string Token;
            while(getline(StrStream, Token, ' '))
                words.push_back(Token);

            query q;
            if(words.at(0).compare("select")==0)
            {    
                q.type = "select";


                int df=0,cf=0,of=0,gf=0,hf=0,j;
                for(int i=0;i<words.size()-1;i++)
                {
                    j = i+1;
                    if(words.at(i).compare("from")==0)
                    {
                        while(j<words.size()&&words.at(j).compare("where")!=0&&words.at(j).compare("groupby")!=0&&words.at(j).compare("orderby")!=0&&words.at(j).compare("having")!=0)
                            q.tables.append( words.at(j++) );
                    }
                    else if(words.at(i).compare("select")==0)
                    {
                        while(j<words.size()&&words.at(j).compare("from")!=0)
                            q.columns.append( words.at(j++) ); 
                    }
                    else if(words.at(i).find("distinct")==0)
                    {
                        df=1;
			int zz=0;
			for(zz=0;zz<words.at(i).length();zz++)
				if( words.at(i)[zz] == '(' )
					break;
			int aa;
			for(aa=zz+1;aa<words.at(i).length();aa++)
				if( words.at(i)[aa] == ')' )
					break;
			q.distinct = words.at(i).substr(zz+1,aa-zz-1);
                    }
                    else if(words.at(i).compare("where")==0)
                    {
                        cf=1; 
                        while(j<words.size() && (words.at(j).compare("groupby")!=0&&words.at(j).compare("having")!=0&&words.at(j).compare("orderby")!=0) )
                            q.condition.append( words.at(j++) );
                    }
                    else if(words.at(i).compare("orderby")==0)
                    {
                        of=1;
                        while(j<words.size())
                            q.orderby.append( words.at(j++) );
                    }
                    else if(words.at(i).compare("groupby")==0)
                    {
                        gf=1;
                        while(j<words.size()&&words.at(j).compare("having")!=0&&words.at(j).compare("orderby")!=0)
                            q.groupby.append( words.at(j++) );
                    }
                    else if(words.at(i).compare("having")==0)
                    {
                        hf=1;
                        while(j<words.size()&&words.at(j).compare("orderby")!=0)
                            q.having.append( words.at(j++) );
                    }
                }
                if(df==0)
                    q.distinct.append( "NA" );
                if(cf==0)
                    q.condition.append( "NA" );
                if(of==0)
                    q.orderby.append( "NA " );
                if(gf==0)
                    q.groupby.append( "NA" );
                if(hf==0)
                    q.having.append( "NA" );
                if(q.columns.compare("*")==0)
                {
                    /*q.columns="";
                    for( int i=0;i<table_index; i++ )
                        for( int j=0; j<tables[i].num; j++ )
                        {
                            q.columns.append( tables[i].attribute[j] );
                            q.columns.append( "," );
                        }*/
                }
            }
            else if(words.at(0).compare("create")==0)
            {
                int j;
                q.type="create";
                for(int i=0;i<words.size()-1;i++)
                {
                    j = i+1;
                    if(words.at(i).compare("table")==0)
                    {
                        q.tables.append( words.at(j++) );

                        while(j<words.size())
                        {
                            q.attributes.append(words.at(j));
                            q.attributes.append(" ");
                            j++;

                        }
                    }
                }
            }
            words.clear();
            return q;
        }
        ////////////////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////////////////
        int validate( query q )
        {
            if((q.type).compare("select")==0)
            {    
                int tf = 1, cf = 1, wf = 0, gf = 0, hf = 0, of = 0;

                for(int i=0; i<table_index; i++)
                {
                    for(int j=0; j<tables[i].num; j++)
                    {
                        if( q.condition.find( tables[i].attribute[j] ) != string::npos ) wf = 1;
                        if( q.groupby.find( tables[i].attribute[j] ) != string::npos ) gf = 1;
                        if( q.having.find( tables[i].attribute[j] ) != string::npos ) hf = 1;
                        if( q.orderby.find( tables[i].attribute[j] ) != string::npos ) of = 1;
                    }
                }

                istringstream s1(q.tables);
                string token;
                while( getline( s1, token, ',' ) )
                {
                    int bit = 0;

                    string tmp = filePath;
                    tmp.append(token);
                    token = tmp;
//		    cout << token << endl;
                    for(int i=0;i<table_index;i++)
{
                        if( tables[i].name == token )
                            bit = 1;
}
                    tf = tf && bit;
                }

                if( q.columns.compare("*") != 0 )
                {
                    istringstream s2(q.columns);
                    while( getline( s2, token, ',' ) )
                    {
                        int bit = 0;
                        for(int i=0;i<table_index;i++)
                            for(int j=0; j<tables[i].num; j++)
                            {
                                string s = string(tables[i].attribute[j]);
                                if( token.find( s ) != string::npos )
                                    bit = 1;
                            }
                        cf = cf && bit;
                    }
                }
	
		//icout << tf << " " << cf << " " << wf "" << gf << " " << hf << " " << of << endl;
//		cout << tf << " " << cf << " " << wf << " " << gf << " " << hf << " " << of << endl;                
		return ( tf && cf && wf && gf && hf && of );
                return 1;
            }
            if((q.type).compare("create")==0)
            {
                int bit = 1;
                string tmp=filePath;
                tmp.append( q.tables );
                tmp.append(".csv");
                for(int i=0;i<table_index;i++)
                    if( tables[i].name == tmp )
                        bit = 0;

                return bit; 

            }
        }
        //////////////////////////////////////////////////////////////////////


        //////////////////////////////////////////////////////////////////////
        void selectCommand(string input)
        {
            query q = parse( input );
            int bit = validate( q );

            if( bit )
                printQuery(q);
            else
                cout << "Invalid Query\n";
        }

        void createCommand(string input)
        {
            query q=parse(input);

            int bit=validate(q);
            if(bit)
            {

                string tmp=q.tables;
                string x=tmp.c_str();
                x.append(".csv");

                FILE *f;
                f=fopen(x.c_str(),"w");
                fclose(f);

                string data=tmp.append(".data");
                FILE *fd=fopen(data.c_str(),"w");
                q.attributes.erase(0,1);
                q.attributes.erase(q.attributes.size()-2,1);
                fputs(q.attributes.c_str(),fd);
                fclose(fd);

                char buff[100];
                FILE *np=fopen(data.c_str(),"r+");
                fclose(np);


                printQuery(q);

                FILE *config=fopen("config.txt","a+");
                fputs("BEGIN\n",config);
                fputs(q.tables.c_str(),config);
                fputc( '\n', config );

                string token;
                istringstream ss( q.attributes );
                while( getline( ss, token, ',' ) )
                {
                    string t1;
                    int zz;
                    for(zz=0;zz<token.length();zz++)
                        if( token[zz] == ' ' )
                            break;
                    t1 = token.substr(0,zz);
                    string t2 = token.substr(zz+1,token.length()-zz);
                    cout << t1 << " " << t2 << endl;
                    fputs( t1.c_str(), config);
                    fputc( ',', config);
                    fputs(t2.c_str(),config);
                    fputc( '\n', config );
                }

                fputs("END\n",config);
                fclose(config);
            }
            else
                cout<<"Invalid Query\n";
        }
        ////////////////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////////////
        void queryType(string input)
        {
            char p[100];
            string Token;
            istringstream StrStream(input);

            getline(StrStream, Token, ' ');
            if(Token.compare("select")==0)
                selectCommand(input);
            else if(Token.compare("create")==0)
                createCommand(input);
        }
        ///////////////////////////////////////////////////////////////


};

int main()
{
    DBSystem file1;
    file1.readConfig("config.txt");
    file1.populateDBInfo();
//initial

/*
file1.queryType("create table test1 (id int,fname string,lname string,percent float,class string,year int)");
file1.queryType("create table test2 (id int,fname string,percent float)");
file1.queryType("create table test3 (id int,lname string,percent float)");
//create
file1.queryType("create table dbs (id int,roll_no int,name string)");
file1.queryType("create table t1 (temp int,temp int)");
file1.queryType("create table t1 (temp int,temp float)");
file1.queryType("create table t1 (temp int,temp int)");
file1.queryType("create table dbs (temp int,temp int)");
file1.queryType("create table t1 (varchar int,temp int)");
file1.queryType("create table t1 (float int,temp int)");
file1.queryType("create table t1 (temp int,temp1 float)");
//from
*/

file1.readConfig("config.txt");
file1.queryType("select roll_no from dbs where id=1");
file1.queryType("select * from dbs");
file1.queryType("select * from dbs where id>=1");
file1.queryType("select * from abc");
file1.queryType("select * from test1,test1");
file1.queryType("select * from test1,test2,test3");
//select

file1.queryType("select abc from test1");
file1.queryType("select abc from test1,test2,test3");
file1.queryType("select id,fname from test1");
file1.queryType("select test1.id from test1");
file1.queryType("select test2.id from test1");
file1.queryType("select test2.fname from test1,test2,test3");
file1.queryType("select id,fname from test1,test2,test3");
file1.queryType("select id,test2.fname,lname from test1,test2,test3");
file1.queryType("select * from test1");
file1.queryType("select * from test1,test2,test3");
file1.queryType("select *,id from test1");
file1.queryType("select *,id,fname from test1,test2,test3");
file1.queryType("select null from test1");
file1.queryType("select null");
file1.queryType("select distinct(id),fname,lname from test1");
file1.queryType("select distinct(id),fname,lname from test1");
//file1.queryType("select distinct(id,fname,lname) from test1");
file1.queryType("select distinct(fname),abc from test1");
file1.queryType("select distinct(test2.lname) ,fname from test1,test2");
//where

file1.queryType("select * from test1 where id>5.5");
file1.queryType("select * from test1 where percent>10");
file1.queryType("select * from test1 where id like 5.5");
file1.queryType("select * from test1 where percent like 10");
file1.queryType("select * from test1 where fname = 'abcd'");
file1.queryType("select * from test1 where fname like 23");
file1.queryType("select * from test1 where fname like 34.5");
file1.queryType("select * from test1 where id like 'sdffs'");
file1.queryType("select * from test1 where percent like 'sdffs'");
file1.queryType("select * from test1 where id<32492752938523958237529");
file1.queryType("select * from test1 where percent<32492752938523958237529");
file1.queryType("select * from test1,test2 where lname='abcd')");
file1.queryType("select * from test1,test2 where id=1");
file1.queryType("select * from test1 where roll=45");
file1.queryType("select * from test1,test2 where roll=34");
file1.queryType("select * from test1,test2 where test1.id=1");
file1.queryType("select * from test1,test2 where (id)=2");
file1.queryType("select * from test1 where id<3 and percent<30");
file1.queryType("select * from test1 where id<3 AND percent<30");
file1.queryType("select * from test1 where id>3 OR percent<30");
file1.queryType("select * from test1 where id>3 or percent<30");
file1.queryType("select * from test1 where lname like 'cdef'");
file1.queryType("select * from test1 where id<=3 and percent>30 or fname like 'abce'");
file1.queryType("select * from test1 where id>5 and id<5 and id>=5 and id<=5 and id=5 and id<>5");
file1.queryType("select * from test1 where percent>13.5 and percent<13.5 and percent>=13.5 and percent<=13.5 and percent<>13.5 and percent=13.5");

//group by
file1.queryType("select roll from test1 groupby roll");
file1.queryType("select id,fname from test1 groupby class");
file1.queryType("select class from test1 groupby (class)");
file1.queryType("select class,year from test1 groupby class,year");
file1.queryType("select test1.class from test1 groupby test1.class, test1.year");
//having

file1.queryType("select year from test1 groupby class having class like 'B'");
file1.queryType("select class from test1 groupby class having year=2012");
file1.queryType("select fname from test1 having id<=3");
file1.queryType("select id from test1 having id<=3");
//order by
file1.queryType("select * from test1 orderby roll");
file1.queryType("select id from test1 orderby percent");
file1.queryType("select id,percent from test1 orderby (percent)");
file1.queryType("select * from test1 ordeby year");
file1.queryType("select test1.id,test1.percent,test1.year from test1 orderby percent,year");
file1.queryType("select test1.id,test1.percent,test1.year from test1 orderby year,percent");

   /* while(1)
    {
        cout<<"Enter query\n";
        char Q[100],ch;
        scanf("%[^\n]",Q);
        scanf("%c",&ch);
        string s=string(Q);
        file1.queryType(s);
    }*/
    // file1.queryType("select * from table1  , table2 where table1.e_id=5 groupby s_id having table1.e_order=50 orderby s_num");
    //   file1.queryType("create table <table1> (<col1> <datatype>,<col2> <datatype>)");
    return 0;
}
