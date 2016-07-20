#include <stdio.h>
#include <stdlib.h>
#include "nobj.h"
typedef int bool;
#define true 1
#define false 0

unsigned int ** parse_nobj_file(char * file, struct nobj_meta *nobj_props) {
  FILE *fp = fopen(file,"r");
  char buff[255];
  int t;
  bool has_neur_settings=false;
  bool has_neur_count=false;
  int max=10;
  int i =0;
  /* Loops until all meta data is read setting:\s val */
  while(!(has_neur_settings&&has_neur_count)) {
    i++;
    //printf("bools %d   and   %d",has_neur_settings,has_neur_count);
    fscanf(fp,"%[^:]:%u ",buff,&t);
    printf(" READ (%s) %s %d\n",file,buff,t);
    if(strcmp(buff,"non")==0) {
      (*nobj_props).num_of_neurs=t;
      has_neur_count=true;
    } else if(strcmp(buff,"np")==0){
      (*nobj_props).num_of_neur_properties=t;
      has_neur_settings=true;
    } else {
      printf("Unkown setting %s in %s\n",buff,file);
    }

    if(i>=max) { 
       printf("ERROR READING %s\n",file);
       fclose(fp);
       return NULL;
    }
  }  
  /* Read all neurs  */
  unsigned int**props=malloc(sizeof(unsigned int*) * (*nobj_props).num_of_neurs);
  int n =0;
  for(n;n<(*nobj_props).num_of_neurs;++n) {
    int p = 0;
    props[n]=malloc(sizeof(unsigned int) * (*nobj_props).num_of_neur_properties);
    for(p;p<(*nobj_props).num_of_neur_properties;++p) {
      if(fscanf(fp,"%u%*[, \t\n]",&props[n][p])<1) {
        printf("ERROR READING %s : MISSING AT LEAST 1 NEUR PROPERTY\n",file);
        fclose(fp);
        return NULL;
      }
      //printf("  props:%u\n",props[n][p]);
    }
  }
  fclose(fp);
  return props;
}
//initilizes(inserts) object at index no with non neurs with props[] properties
void init_nobj(int no, unsigned int** props, struct nobj_meta obj_prop, unsigned int ****nobj) {
  (*nobj)[no]=malloc(sizeof(unsigned int*)*obj_prop.num_of_neurs); //initilize obj's array of neurs

  if( !(*nobj)[no]) {
    printf("ERROR - failed to malloc nobj\n");
    return;
  }

  int i = 0;
  for(i;i<obj_prop.num_of_neurs;++i) {
    (*nobj)[no][i]=malloc(sizeof(unsigned int)*obj_prop.num_of_neur_properties);
    
    int j = 0;
    for(j;j<obj_prop.num_of_neur_properties;++j) {
      
      ((*nobj)[no][i][j])=props[i][j];
     
    }
  }
}
void free_nobj(int no, struct nobj_meta obj_prop, unsigned int ****nobj){
 
  int i = 0;
  for(i;i<obj_prop.num_of_neurs;++i) {
    free((*nobj)[no][i]);
  }
  free((*nobj)[no]);

}


unsigned int ** parse_con_file(char * file,struct nobj_meta *nobj_props) {
  FILE *fp = fopen(file,"r");
  char buff[255];
  unsigned int t;//temp var for reading header settings
 
  bool has_con_count=false;
  bool has_con_properties=false;
  int max_header_lines=10;
  int i =0;
  /* Loops until all meta data is read setting:\s val */
  while(!(has_con_count&&has_con_properties)) {
    i++;
    //printf("bools %d   and   %d",has_neur_settings,has_neur_count);
    fscanf(fp,"%[^:]:%u ",buff,&t);
    printf(" READ (%s) %s %d\n",file,buff,t);

    if(strcmp(buff,"con")==0) {
      (*nobj_props).num_of_cons=t;
       has_con_count=true;

    } else if(strcmp(buff,"cp")==0) {
      (*nobj_props).num_of_con_properties=t;
      has_con_properties=true;

    }else {
      printf("Unkown setting %s in %s\n",buff,file);
    }

    if(i==max_header_lines) { 
       printf("ERROR READING %s\n",file);
       fclose(fp);
       return NULL;
    }
  }
  
  /* Read all neurs  */
  unsigned int**cons=malloc(sizeof(unsigned int*) * (*nobj_props).num_of_cons);
  unsigned int n =0;
  for(n;n<(*nobj_props).num_of_cons;++n) {
    int p = 0;
    cons[n]=malloc(sizeof(unsigned int) * (*nobj_props).num_of_con_properties);
    for(p;p<(*nobj_props).num_of_con_properties;++p) {
      if(fscanf(fp,"%u%*[, \t\n]",&cons[n][p])<1) {
        printf("ERROR READING %s : MISSING AT LEAST 1 con\n",file);
        fclose(fp);
        return NULL;
      }else{}   
      //printf("  cons[%u] prop val:%u\n",n,cons[n][p]);
    }
     if(cons[n][0]>(*nobj_props).num_of_neurs-1 || cons[n][1]>(*nobj_props).num_of_neurs-1 ) {
        printf("ERROR Reading con file, reference to invalid neur: from[%u] or to[%u]\n",cons[n][0],cons[n][1]);
        return NULL;
      }
      
  }
  fclose(fp);
  return cons;
}
int init_cons(int no, unsigned int** con_props, struct nobj_meta obj_prop, unsigned int ****cons, unsigned int ****conids,double ****weights) {
   
  if(weights[no]==NULL) {
    printf("Error: WEIGHTS ARRAY IS NULL\n");
    exit(-1);
  }
  (*cons)[no]   =malloc(sizeof(  unsigned int*)*obj_prop.num_of_cons); //initilize obj's array of neurs
  (*conids)[no] =malloc(sizeof(  unsigned int*)*obj_prop.num_of_cons); 
  (*weights)[no]=malloc(sizeof(        double*)*obj_prop.num_of_cons); 

  if( !((*cons)[no]||(*conids)[no]||(*weights)[no]) ) {
    printf("ERROR - failed to malloc at least 1 of 3 con arrays\n");
    return -1;
  }
  unsigned int num_of_cons[obj_prop.num_of_neurs];//holds total neur count
  unsigned int num_of_cons_rec[obj_prop.num_of_neurs];//holds total receiving cons
  unsigned int num_of_cons_ass[obj_prop.num_of_neurs];//tmp used for array init, holds amount assigned
  unsigned int nocw[obj_prop.num_of_neurs];//tmp used for indexing - numc of cons weight
  unsigned int i = 0;
  //init array to 0
  for(i;i<obj_prop.num_of_neurs;++i) {
    //these arrays used to to track the size of other arrays
    //init to 1 since first element on cons,conids,weights array store legnth of array
    //total array length is (array[0]+1)
    num_of_cons[i]=0;
    num_of_cons_ass[i]=1;
    num_of_cons_rec[i]=0;
    nocw[i]=1;
  }
  

  i = 0;
  //tally array members
  for(i;i<obj_prop.num_of_cons;++i) {
    if(con_props[i][0]<obj_prop.num_of_neurs) {//make sure its not an invalid indexx
      num_of_cons[con_props[i][0]]++;
      num_of_cons_rec[con_props[i][1]]++;
    } else { 
      printf("ERROR LOADING CON FILE, REFERENCE TO NONEXISTENT NEUR\n");
      return -1;
    }
  }
  i=0;
  //printf("here!!!: %u\n",obj_prop.num_of_neurs);
  //allocate space for each array for each neur for the num of cons each neur has
  for(i;i<obj_prop.num_of_neurs;++i) {
    (*cons)[no][i]=malloc(sizeof(unsigned int)* (num_of_cons[i]+1));
    //printf("NEUR %u CON LENGTH %u\n", i, num_of_cons[i]);
    (*cons)[no][i][0]=num_of_cons[i];//first element in array specifies number of cons
    (*conids)[no][i]=malloc(sizeof(unsigned int)* (num_of_cons[i]+1));
    (*conids)[no][i][0]=num_of_cons[i];//first element in array specifies number of cons
    (*weights)[no][i]=malloc(sizeof(double)* (num_of_cons_rec[i]+1));   
    (*weights)[no][i][0]=num_of_cons_rec[i];//first element in array specifies number of weights
    //printf("malloc for nobj %u for receiving nuer %u , rec count %u\n",no,i,num_of_cons_rec[i]);
  }

  
  i=0; 
  for(i;i<obj_prop.num_of_cons;++i) {
    ((*cons   )[no][con_props[i][0]][ num_of_cons_ass[con_props[i][0]] ]) = con_props[i][1];
    ((*conids )[no][con_props[i][0]][ num_of_cons_ass[con_props[i][0]] ]) = nocw[con_props[i][1]];
    ((*weights)[no][con_props[i][1]][ nocw           [con_props[i][1]] ]) = (double)(con_props[i][2]/100.0);


    //printf("  NEUR %u , CON[%u] , TO %u\n",con_props[i][0],(num_of_cons_ass[con_props[i][0]]),con_props[i][1]);
    //printf("-----CONID: %u\n",nocw[con_props[i][1]] );
    //printf(" ----WEIGHT: %lf\n",((*weights)[no][con_props[i][1]][ nocw[con_props[i][1]] ]));
    num_of_cons_ass[con_props[i][0]]++;//incrementr num of cons sender has in con array
    nocw[con_props[i][1]]++;
  }

}
void display_con_props(int obj,unsigned int***cons,unsigned int***conids,double***weights,struct nobj_meta *np) {
  unsigned int i =0,j=1;
  printf("OBJ[%d]\n",obj);
  printf("  Total cons: %u\n",np[obj].num_of_cons);

  for(i;i<np[obj].num_of_neurs;++i) {
    j=1;
    printf("  Neur[%u] num of cons:%u\n",i,cons[obj][i][0]);

    for(j;j<cons[obj][i][0]+1;++j){
      printf("  Neur[%u] conto:%u\n",i,cons[obj][i][j]);
      printf("      conid: %u\n"    ,conids[obj][i][j]);
      printf("      weight:%lf\n"   ,weights[obj][cons[obj][i][j]][ conids[obj][i][j] ]  );
    }
  }
}
void display_neur_props(int obj,unsigned int***nobjs,struct nobj_meta *np) {
  unsigned int i =0,j=0;
  printf("OBJ[%d]\n",obj);
  printf("  Total neurs: %u\n",np[obj].num_of_neurs);

  for(i;i<np[obj].num_of_neurs;++i) {
    j=0;
    for(j;j<np[obj].num_of_neur_properties;++j){
      printf("  Neur[%u] prop[%u] val:%u\n",i,j,nobjs[obj][i][j]);
      
    }
  }
}
void stim(int nobj_id,unsigned int neur_from, unsigned int neur_to, unsigned int conid, double stim,
 struct behav_pool bp,unsigned int***nobj,unsigned int***cons,unsigned int***conids, double***weights,
 double***vars) {
  //PRE
  bp.behaviors[nobj[nobj_id][neur_to][0]];
  //vars[nobj_id][neur_to][0]+=//weights LOH 7-16-16


}
