#include "LinSetClass.h"
#include <string>
setClass::setClass(){
    total = 0;
    for (int i = 0; i < hashArrayMaxSize; i++)
    {
        hashItemArray[i] = NULL;
    }
    for (int i = 0; i < shareSet; i++)
    {
        statistic[i] = 0;
        outputStatistic[i]=0;
        outputstring[i]="";
        vertical[i]=NULL;
    }
    for(int i=0;i<maxNonraph;i++){
        Head[i]=NULL;
        outPutHead[i]=NULL;
    }
}
setClass::setClass(QString fileList)
{
    total = 0;
    for (int i = 0; i < hashArrayMaxSize; i++)
    {
        hashItemArray[i] = NULL;
    }
    for (int i = 0; i < shareSet; i++)
    {
        statistic[i] = 0;
        outputStatistic[i]=0;
        outputstring[i]="";
        vertical[i]=NULL;
    }
    for(int i=0;i<maxNonraph;i++){
        Head[i]=NULL;
        outPutHead[i]=NULL;
    }
    addFile(fileList);
}

setClass::~setClass()
{
	for (int i = 0; i < total; i++)
	{
        outPutHead[i]=NULL;
		delete Head[i];
	}
	for (int i = 0; i < hashArrayMaxSize; i++)
	{
		if (hashItemArray[i])
		{
            itemName *q;

            while(hashItemArray[i]){
                q=hashItemArray[i]->next;
                delete hashItemArray[i];
                hashItemArray[i]=q;
            }
		}
	}
}

//Traverse the latest file
void setClass::traversal()
{
	int id=0,seq=0;
	std::ifstream file;
    std::string item;
    file.open(Head[total-1]->absaddr.c_str(), std::ios::in);
    id = Head[total-1]->id;
    if (file.fail())
    {
        QString temp("Can not open the following file:");
        temp.append(Head[total-1]->absaddr.c_str());
        QMessageBox::critical(NULL,QString("Error"),temp);
        return;
    }
    file >> item;
    while (!file.eof())
    {
        seq = BKDRHash(item.c_str());
        fillHashItemArray(seq, id, item);
        file >> item;
    }
    if(!item.empty()){
        seq = BKDRHash(item.c_str());
        fillHashItemArray(seq, id, item);
    }
    file.close();
}


// Hash Function
unsigned int setClass::BKDRHash(const char *item)
{
    unsigned int seed = 2531; // modify seed to a primate 2531
	unsigned int hash = 0;

	while (*item)
	{
		hash = hash * seed + (*item++);
		hash = hash%hashArrayMaxSize;
	}

	return (hash & 0x7FFFFFFF);
}

/*init the item array */
void setClass::fillHashItemArray(int seq, int id, std::string item)
{   
    itemName *p,*q;
    p=hashItemArray[seq];
    q=p;
    while(p){
        if(p->item.compare(item)){
            p=p->next;
        }
        else{
            if (!((1<<(id-1))&(p->list)))//if item already exists add file info to list
            {
                p->list = p->list + (1 << (id - 1));
            }
            return;
        }
    }
    hashItemArray[seq]=new itemName;//if can not find item in the hashtable new one
    hashItemArray[seq]->item=item;
    hashItemArray[seq]->list = hashItemArray[seq]->list + (1 << (id - 1));
    hashItemArray[seq]->next=q;
} 

//census selected lists
void setClass::statistica(int list){
    for(int i=0;i<shareSet;i++){
        statistic[i]=0;
        itemName *p;
        p=vertical[i];
        while (p) {
            vertical[i]=vertical[i]->line;
            p->line=NULL;
            p=vertical[i];
        }
    }
    int templist=0;
	for (int i = 0; i < hashArrayMaxSize; i++)
	{
		if (hashItemArray[i])
		{
            itemName *q,*p;
            q=hashItemArray[i];
            while(q){
                templist=list&(q->list);
                statistic[templist]++;
                p=vertical[templist];
                vertical[templist]=q;
                vertical[templist]->line=p;
                q=q->next;
            }
		}
	}
}

// addFile and trim the file name from full path
int setClass::addFile(QString fileList){
   int state=0;
   int pos=0;
   std::string fileListstr,filename;
   QByteArray ba = fileList.toLocal8Bit(); // using local encode to avoid errors
   const char *tempfile;
   tempfile=ba;
   fileListstr=tempfile;
   while ((pos=fileListstr.find_first_of('\r'))>=0) {
        filename=fileListstr.substr(0,fileListstr.find_first_of('\r'));
        fileListstr.erase(0,fileListstr.find_first_of('\r')+1);
        state=initFileIfo(filename.c_str());
        if(state<0)
        {
            return state;
        }
   }
    state=initFileIfo(fileListstr.c_str());
    return state;
}

// stores fileInfo with Head array
int setClass::initFileIfo(const char *tempfile){
    int state=0;//
    int pos=0;
    int flag;
    total++;
    if(total>maxSamples&&total<10){

       flag=QMessageBox::question(NULL,QString("Warnings"),QString("You have loaded over 8 sets.Swith to Matrix From!"),QMessageBox::Yes,QMessageBox::No);
       if(flag==QMessageBox::No){
           total--;
           state=EIGHTONLY;
           return state;
           }
    }
    if(total>maxNonraph){

       QMessageBox::warning (NULL,QString("Warnings"),QString("You have loaded over 31 sets.Supernumerary sets will be ignored?"),QMessageBox::Ok);
        //
       state=OVERLOADFILE;
       total--;
       return state;
    }
    Head[total-1]=new fileIfo;
    if(Head[total-1]==NULL){
        QMessageBox::critical(NULL,QString("Error"),QString("Can not allocate memory!"));
        state=ALLOCATEMEMORYFAILED;
        return state;
    }
    Head[total-1]->id=total;
    Head[total-1]->absaddr=tempfile;
    Head[total-1]->name=tempfile;
    pos=Head[total-1]->name.find_last_of('\\');
    if(pos>=0){
        Head[total-1]->name.erase(0,pos+1);
    }

    pos=Head[total-1]->name.find_last_of('/');
    if(pos>=0){
        Head[total-1]->name.erase(0,pos+1);
    }

    pos=Head[total-1]->name.find_last_of('.');
    if(pos>=0){
        if(Head[total-1]->name.find(' ',pos)==std::string::npos)
            Head[total-1]->name.erase(pos);
    }

    traversal();
    return state;
}

 void setClass::transoutput(int list){//将不标准的格式转换成linSvg的标准输出格式
     int tempList=0,trans=0,recordnoSelect[maxNonraph+1];
     //list=(list<<1)&0x7FFFFFFF;
     for(int i=0,j=list,k=0,l=0,m=0;i<maxNonraph+1;i++){//记录哪些set被选中
         recordnoSelect[i]=INT_MAX;
         if(!(k=j%2)){
              recordnoSelect[l]=recordnoSelect[l]<<(i+1-l);
              recordnoSelect[l]=recordnoSelect[l]&0x7FFFFFFF;
                l++;
         }
         else {
             if(m<maxNonraph){
                 outPutHead[m]=Head[i];
                 m++;
             }

         }
         j=j/2;
     }
     std::string unions="∪";
     std::string Intersection="∩";
     for(int i=1,l=0;i<=list;i++){
         if((tempList=i&list)){
             l=tempList;
             for(int j=0;j<total;j++){
                  trans=trans+((INT_MAX-recordnoSelect[j])&l);
                  l=(l&recordnoSelect[j])>>1;
             }
             outputStatistic[trans]=statistic[tempList];
             int rest=findunit(trans,list);
             std::string tmpcontext,tmpcontextr;
             for(int i=0;i<total;i++){
                 if(list&1<<i){
                    if(rest&(1<<i)){
                        tmpcontext=tmpcontext+Intersection+Head[i]->name;
                    }else{
                        tmpcontextr=tmpcontextr+unions+Head[i]->name;
                    }
                 }
             }
             if(tmpcontext.find(Intersection.c_str())==0){
                 tmpcontext.replace(0,Intersection.length(),"");
             }
             if(tmpcontextr.find(unions.c_str())==0){
                 tmpcontextr.replace(0,unions.length(),"");
             }
             if(tmpcontextr==""){
                 outputstring[trans]=tmpcontext;
             }else{
                 if(tmpcontext.find(Intersection.c_str())==std::string::npos)
                    outputstring[trans]=tmpcontext+"\\";
                 else
                    outputstring[trans]="("+ tmpcontext+")\\";
                 if(tmpcontextr.find(unions.c_str())==std::string::npos)
                     outputstring[trans]+=tmpcontextr;
                 else
                     outputstring[trans]+="("+tmpcontextr+")";
             }

             trans=0;
         }
     }

 }

 //根据上一个transoutput函数反推出哪些item在list里，即将VennID转换成真正的ID
 int setClass::findunit( int vennId, int list){
     int tempList=0,trans=0,recordnoSelect[maxNonraph+1];
     //list=(list<<1)&0x7FFFFFFF;
     for(int i=0,j=list,k=0,l=0;i<maxNonraph+1;i++){//记录哪些set被选中
         recordnoSelect[i]=INT_MAX;
         if(!(k=j%2)){
              recordnoSelect[l]=recordnoSelect[l]<<(i+1-l);
              recordnoSelect[l]=recordnoSelect[l]&0x7FFFFFFF;
                l++;
         }
         j=j/2;
     }
     for(int i=1,l=0;i<=list;i++){
         if((tempList=i&list)){
             l=tempList;
             for(int j=0;j<total;j++){
                  trans=trans+((INT_MAX-recordnoSelect[j])&l);
                  l=(l&recordnoSelect[j])>>1;
             }
             if(vennId==trans){
                 return tempList;
             }
             trans=0;
         }
     }
     return 0;
 }


 void setClass::exportsharesets(QString filename,int list){
  QByteArray  filenm=filename.toLocal8Bit();
     int tempList=0;
     std::ofstream file;
     file.open(filenm);
     if (file.fail())
     {
         QMessageBox::warning(NULL,"Error!","Cannot open the file!");
         return;
     }
     itemName *p,*q;
     for (int i = 1,k=0,l=0,pre=0; i <= list; i++)
     {
         if((tempList=i&list)){
             if(tempList>pre){
                 pre=tempList;
                 l = tempList;
                 for (int j = 0; j < total; j++)
                 {
                     if ((k = l % 2))
                     {
                         file << Head[j]->name << " ";
                     }
                     l = l / 2;
                 }
                 file <<":\t";
                for(int j=0;j<hashArrayMaxSize;j++){
                    p=hashItemArray[j];
                    q=p;
                    while(p){
                        if(((p->list)&list)==tempList){
                            file<<p->item<<"\t";
                            p=p->next;
                        }
                        else{
                            p=p->next;
                        }
                    }
                }
                 file << std::endl;
               }
         }

     }
     file.close();
 }

 void setClass::exportvertical(QString filename,int list)
 {
     QByteArray  filenm=filename.toLocal8Bit();
    int tempList=0;
    int Maxlin=0;
    std::ofstream file;
    file.open(filenm);
    if (file.fail())
    {
        QMessageBox::warning(NULL,"Error!","Cannot open the File!");
        return;
    }
    itemName *p;
    for (int i = 1,k=0,l=0,pre=0; i <= list; i++)
    {
        if((tempList=i&list)){
            if(tempList>pre){
                pre=tempList;
                l = tempList;
                for (int j = 0; j < total; j++)
                {
                    if ((k = l % 2))
                    {
                        file << Head[j]->name << " ";
                    }
                    l = l / 2;
                }
                file <<"\t";
              }
        }
    }
    file << std::endl;
    for(int i=1;i<shareSet;i++)
    {
        if(Maxlin<statistic[i])
        {
            Maxlin=statistic[i];
        }
    }
    for(int i=0;i<Maxlin;i++){
        for(int j=0,pre=0;j<=list;j++){
            tempList=j&list;
            if(tempList)
            {
                if(tempList>pre){
                    pre=tempList;
                    p=vertical[tempList];
                    if(p){
                        vertical[tempList]=vertical[tempList]->line;
                        p->line=NULL;
                        file<<p->item;
                    }
                    file<<"\t";
                  }
            }
        }
        file<<std::endl;
    }
    statistica(list);
    file.close();
 }

 void setClass::exportMatrixs(QString filename,int list){
      QByteArray  filenm=filename.toLocal8Bit();
      int tempList=0;
      FILE* file;
      file=fopen(filenm,"w");
      if(!file){
          QMessageBox::warning(NULL,"Error!","Cannot open the File!");
          return;
        }
      for(int j=0;j<maxNonraph;j++){
          if(((1<<j)&list)){
              fprintf(file,"\t%s",Head[j]->name.c_str());
            }
        }
      fprintf(file,"\n");
      itemName *p;
      for(int i=0;i<hashArrayMaxSize;i++)
        {
            p=hashItemArray[i];
            while (p) {
                tempList=((p->list)&list);
                if(tempList){
                    fprintf(file,"%s",p->item.c_str());
                   for(int j=0;j<maxNonraph;j++){
                       if(((1<<j)&list)){
                           if(tempList&(1<<j)){
                                fprintf(file,"\t%d",1);
                             }
                           else {
                               fprintf(file,"\t%d",0);
                           }
                       }
                     }
                  }
                fprintf(file,"\n");
                p=p->next;
              }
        }
      fclose(file);
  }

 void setClass::Remove(){
     for (int i = 0; i < total; i++)
     {
         outPutHead[i]=NULL;
         delete Head[i];
     }
     for(int i=0;i<shareSet;i++){
         vertical[i]=NULL;
     }
     for (int i = 0; i < hashArrayMaxSize; i++)
     {
         if (hashItemArray[i])
         {
             itemName *q;

             while(hashItemArray[i]){
                 q=hashItemArray[i]->next;
                 delete hashItemArray[i];
                 hashItemArray[i]=q;
             }
         }
     }
     total = 0;
     for (int i = 0; i < hashArrayMaxSize; i++)
     {
         hashItemArray[i] = NULL;
     }
     for (int i = 0; i < shareSet; i++)
     {
         statistic[i] = 0;
         outputStatistic[i]=0;
         outputstring[i]="";
     }
     for(int i=0;i<maxNonraph;i++){
         Head[i]=NULL;
         outPutHead[i]=NULL;
     }
 }
